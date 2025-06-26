#pragma once

#include <assert.h>
#include <limits>

namespace sf {

/**
 * Base of SoundIn and SoundOut.
 */
class SoundBase
{
public:
    /**
     * No of channels.
     */
    virtual int noChannels() = 0;

    /**
     * Reset internal buffers.
     */ 
    virtual void reset() = 0;
};

/**
 * Something you can input samples into. 
 */
class SoundIn : public SoundBase
{
public:

    /**
     * Total number of samples available for writing.
     * 
     * @return Number of samples available
     */
    unsigned int writeAvail() {
        return priWriteSize() + secWriteSize();
    }

    /**
     * Commits the number of written samples advancing the buffer pointer 
     * by that many samples.
     * 
     * @param samples       Number of samples
     */
    virtual bool commit(unsigned int samples) = 0;

    /** Primary buffer write pointer */
    virtual float * priWritePtr(int ch) = 0;
    
    /** Size of primary write buffer */
    virtual unsigned int priWriteSize() = 0;

    /** Secondary buffer write pointer (optional, may return null) */
    virtual float * secWritePtr(int ch) { return nullptr; };
    
    /** Size of secondary write buffer, if 0 is returned secWritePtr may return null */
    virtual unsigned  int secWriteSize() { return 0; };

    /**
     * Writes a single sample to the specified channel.
     * 
     * When not compiled with debug this function silently fails if `ch` is not
     * correct, or pos >= writeAvail().
     * 
     * @param   ch      The channel number
     * @param   pos     The position in the buffer (< writeAvail())
     * @param   value   The value to write
     */
    void write(unsigned int ch, unsigned int pos, float value)
    {
        assert (ch < noChannels());
        if (pos < priWriteSize()) {
            priWritePtr(ch)[pos] = value;
            return;
        }
        pos -= priWriteSize();
        if (pos < secWriteSize()) {
            secWritePtr(ch)[pos] = value;
            return;
        }
        assert(false);
    }
};


/**
 * Something you can read samples from.
 */
class SoundOut : public SoundBase
{
public:

    /**
     * Number of samples ready to be read.
     * 
     * @return  Number of samples available
     */
    unsigned int readAvail() {
        return priReadSize() + secReadSize();
    }

    /**
     * Releases the number of read samples
     * 
     * @param samples       Number of samples
     */
    virtual bool release(unsigned int samples) = 0;

    /** Primary buffer read pointer */
    virtual const float * priReadPtr(int ch) = 0;

    /** Size of primary read buffer */
    virtual unsigned int priReadSize() = 0;

    /** Secondary buffer read pointer (optional, may return null) */
    virtual const float * secReadPtr(int ch) { return nullptr; };

    /** Size of secondary read buffer, if 0 is returned secWritePtr may return null */
    virtual unsigned  int secReadSize() { return 0; };

    /**
     * Reads a single sample to the specified channel.
     * 
     * When not compiled with debug this function silently fails if `ch` is not
     * correct, or pos >= writeAvail().
     * 
     * @param   ch      The channel number
     * @param   pos     The position in the buffer (< writeAvail())
     * 
     * @return          The sample value at that position
     */
    float read(unsigned int ch, unsigned int pos)
    {
        assert (ch < noChannels());
        if (pos < priReadSize()) {
            return priReadPtr(ch)[pos];
        }
        pos -= priReadSize();
        if (pos < secReadSize()) {
            return secReadPtr(ch)[pos];
        }
        assert(false);
        return std::numeric_limits<float>::quiet_NaN();
    }
};




}