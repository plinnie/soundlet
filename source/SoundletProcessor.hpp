#pragma once

#include <juce_audio_processors/juce_audio_processors.h>

#include "WaveletFilter.hpp"
#include "soundflowJuceUtils.hpp"

const auto MAX_CHANNELS = 5;
const auto MAX_DEPTH = 8;

//==============================================================================
class SoundletProcessor  : 
    public juce::AudioProcessor, 
    public juce::AudioProcessorValueTreeState::Listener
{
public:
    //==============================================================================
    SoundletProcessor();
    ~SoundletProcessor() override;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;
    using AudioProcessor::processBlock;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    //==============================================================================
    void updateAmplitudeTracking(juce::AudioBuffer<float>& buffer);

    void parameterChanged(const juce::String& parameterID, float newValue) override;

    juce::AudioProcessorValueTreeState & getState()
    {
        return _state;
    }

private:
    //==============================================================================
    juce::AudioProcessorValueTreeState::ParameterLayout  createParameters();
    void processChannel(juce::AudioBuffer<float>& buffer, unsigned int ch);

    void processLevelDecompose(sf::SoundOut& samples_in, sf::SoundIn& detail_out,
                                              sf::SoundIn& approx_out, int depth);
    void processLevelReconstruct(sf::SoundOut& detail_in, sf::SoundOut& approx_in,
                                                sf::SoundIn& samples_out, int depth);

    void reconfigure(const WaveletCoeffs * coeffs, bool sync_params, int buffer_size);

    juce::AudioProcessorValueTreeState _state;
    FilterBank _banks[MAX_DEPTH][MAX_CHANNELS];

    std::atomic<float>* _lowApproxGain = nullptr;
    std::atomic<float>* _detailGains[MAX_DEPTH];
    bool _wavelet_changed;
    const WaveletCoeffs * _coeffs = nullptr;
    bool _sync_buffers = true;
    int _expectedBufSize = 0;

    AudioBufferFifo _detail[MAX_DEPTH];
    AudioBufferFifo _rc_bufs[MAX_DEPTH];
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SoundletProcessor)
};
