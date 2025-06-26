#pragma once

#include "soundflow.hpp"
#include <cassert>
#include <juce_audio_processors/juce_audio_processors.h>

class AudioBufferInputWrapper: public sf::SoundIn
{
private:
    juce::AudioBuffer<float> * _ab;
    unsigned int _offset = 0;

public:
    AudioBufferInputWrapper(juce::AudioBuffer<float> * ab) : _ab(ab) {
         assert(ab != nullptr);
    }

    int noChannels() override { 
        return _ab->getNumChannels(); 
    }

    void setAudioBuffer(juce::AudioBuffer<float> * ab) {
        assert(ab != nullptr);
        _ab = ab;
        _offset = 0;   
    }

    void reset() override
    {
        _offset = 0;
    }

    bool commit(unsigned int samples) override {
        _offset += samples;
        return true;
    }

    float * priWritePtr(int ch) override {
        if (_offset >= _ab->getNumSamples()) {
            return nullptr;
        }

        return _ab->getWritePointer(ch, _offset);
    }

    unsigned int priWriteSize() override {
        return _ab->getNumSamples() - _offset;
    }

};

class AudioBufferOutputWrapper : public sf::SoundOut
{
private:
    juce::AudioBuffer<float>* _ab;
    unsigned int _offset = 0;

public:
    AudioBufferOutputWrapper(juce::AudioBuffer<float>* ab) : _ab(ab) {
        assert(ab != nullptr);
    }

    int noChannels() override { 
        return _ab->getNumChannels(); 
    }

    void setAudioBuffer(juce::AudioBuffer<float>* ab)
    {
        assert(ab != nullptr);
        _ab = ab;
        _offset = 0;
    }

    void reset() override
    {
        _offset = 0;
    }


    bool release(unsigned int samples) override {
        _offset += samples;
        return true;
    }

    const float* priReadPtr(int ch) override {
        if (_offset >= _ab->getNumSamples()) {
            return nullptr;
        }
        return _ab->getReadPointer(ch, _offset);
    }

    unsigned int priReadSize() override {
        return _ab->getNumSamples() - _offset;
    }
};

class AudioBufferFifo : public sf::SoundIn, public sf::SoundOut {

private:
    juce::AudioBuffer<float> _ab;
    unsigned int _capacity;
    unsigned int _commit;
    unsigned int _free;
    unsigned int _rp;
    unsigned int _wp;
    unsigned int _eob_rd;
    unsigned int _eob_wr;
    unsigned int _max_commit_size;

public:
    AudioBufferFifo()
        : _capacity(0), _commit(0), _free(0), _rp(0), _wp(0), _eob_rd(0), _eob_wr(0), _max_commit_size(0)
    {

    }

    void init(int ch, int capacity) 
    {
        _ab.setSize(ch, capacity, false, false, true);
        _ab.clear();
        _capacity = capacity;
        reset();
    }

    void reset() override
    {
        _commit = 0;
        _free = _capacity;
        _eob_rd = _capacity;
        _eob_wr = _capacity;
        _rp = 0;
        _wp = 0;
        _max_commit_size = 0;
    }

    int noChannels() override
    {
        return _ab.getNumChannels();
    }

    // ======================================================
    // From SoundIn
    // ======================================================

    bool commit(unsigned int samples) override 
    {
        if (samples > _free) return false;
        _wp = (_wp + samples) % _capacity;
        _commit += samples;
        _free = _capacity - _commit;
        _eob_wr = _capacity - _wp;
        if (_max_commit_size < _commit) _max_commit_size = _commit;
        return true;
    }

    float * priWritePtr(int ch) override
    {
        return _ab.getWritePointer(ch, _wp);
    }
    
    unsigned int priWriteSize() override
    {
        if (_eob_wr >= _free) return _free;
        return _eob_wr;
    }

    virtual float * secWritePtr(int ch) override 
    {
        if (_eob_wr >= _free) return nullptr;
        return _ab.getWritePointer(ch, 0);
    };
    
    unsigned int secWriteSize() override 
    {
        if (_eob_wr >= _free) return 0;
        return _free - _eob_wr;
    };

    // ======================================================
    // From SoundOut
    // ======================================================

    bool release(unsigned int samples) override
    {
        if (samples > _commit) return false;
        _rp = (_rp + samples) % _capacity;
        _commit -= samples;
        _free = _capacity - _commit;
        _eob_rd = _capacity - _rp;
        return true;
    }

    const float * priReadPtr(int ch) override
    {
        return _ab.getReadPointer(ch, _rp);
    }

    unsigned int priReadSize() override
    {
        if (_eob_rd >= _commit) return _commit;
        return _eob_rd;
    }

    const float * secReadPtr(int ch) override
    {
        if (_eob_rd >= _commit) return nullptr;
        return _ab.getReadPointer(ch, 0);
    }
    
    unsigned  int secReadSize() override
    {
        if (_eob_rd >= _commit) return 0;
        return _commit - _eob_rd;
    }

    unsigned int maxCommitSize() 
    { 
        return _max_commit_size;
    }
};


static inline int consumeToAudioBuffer(juce::AudioSampleBuffer & dst, int offset, sf::SoundOut & so, int noSamples) {
    if (dst.getNumSamples() - offset < noSamples) {
        noSamples = dst.getNumSamples() - offset;
    }

    if (so.readAvail() < noSamples) {
        noSamples = so.readAvail();
    }

    int avail = so.priReadSize();
    if (noSamples <= avail) {            
        // simple case. Fits in complete buffer
        for (int ch = 0; ch < so.noChannels(); ch++) {
            dst.copyFrom(ch, offset, so.priReadPtr(ch), noSamples);
        }
    } else {
        for (int ch = 0; ch < so.noChannels(); ch++) {
            dst.copyFrom(ch, offset, so.priReadPtr(ch), avail);
            dst.copyFrom(ch, offset + avail, so.secReadPtr(ch), noSamples-avail);
        }
    }
    so.release(noSamples);
    return noSamples;
}