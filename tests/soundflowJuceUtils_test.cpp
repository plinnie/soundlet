#include "gtest/gtest.h"
#include "soundflowJuceUtils.hpp"
#include <cstdio>

TEST(AudioBuffer, testFifoSizes) {
    AudioBufferFifo abf;
    abf.init(2, 100);
    EXPECT_EQ(abf.noChannels(), 2);

    EXPECT_EQ(abf.priWriteSize(), 100);
    EXPECT_EQ(abf.secWriteSize(), 0);
    EXPECT_EQ(abf.priReadSize(), 0);
    EXPECT_EQ(abf.secReadSize(), 0);
    EXPECT_EQ(abf.maxCommitSize(), 0);

    abf.commit(50);
    EXPECT_EQ(abf.priWriteSize(), 50);
    EXPECT_EQ(abf.secWriteSize(), 0);
    EXPECT_EQ(abf.priReadSize(), 50);
    EXPECT_EQ(abf.secReadSize(), 0);
    EXPECT_EQ(abf.maxCommitSize(), 50);

    abf.release(25);
    EXPECT_EQ(abf.priWriteSize(), 50);
    EXPECT_EQ(abf.secWriteSize(), 25);
    EXPECT_EQ(abf.priReadSize(), 25);
    EXPECT_EQ(abf.secReadSize(), 0);
    EXPECT_EQ(abf.maxCommitSize(), 50);

    abf.commit(75);
    EXPECT_EQ(abf.priWriteSize(), 0);
    EXPECT_EQ(abf.secWriteSize(), 0);
    EXPECT_EQ(abf.priReadSize(), 75);
    EXPECT_EQ(abf.secReadSize(), 25);
    EXPECT_EQ(abf.maxCommitSize(), 100);

    abf.release(75);
    EXPECT_EQ(abf.priWriteSize(), 75);
    EXPECT_EQ(abf.secWriteSize(), 0);
    EXPECT_EQ(abf.priReadSize(), 25);
    EXPECT_EQ(abf.secReadSize(), 0);
    EXPECT_EQ(abf.maxCommitSize(), 100);
}


TEST(AudioBuffer, testFifoPointers) {
    AudioBufferFifo abf;
    abf.init(2, 100);

    // get base pointers
    float * wr1 = abf.priWritePtr(0);
    const float * rd1 = abf.priReadPtr(0);
 
    ASSERT_TRUE(abf.commit(50));            // rdp=0, wrp=50, c=50
    EXPECT_EQ(abf.priWritePtr(0), wr1+50);
    EXPECT_EQ(abf.secWritePtr(0), nullptr);
    EXPECT_EQ(abf.priReadPtr(0), rd1);
    EXPECT_EQ(abf.secReadPtr(0), nullptr);
    EXPECT_EQ(abf.maxCommitSize(), 50);

    ASSERT_TRUE(abf.release(25));           // rdp=25, wrp=50, c=25
    EXPECT_EQ(abf.priWritePtr(0), wr1+50);
    EXPECT_EQ(abf.secWritePtr(0), wr1);
    EXPECT_EQ(abf.priReadPtr(0), rd1+25);
    EXPECT_EQ(abf.secReadPtr(0), nullptr);
    EXPECT_EQ(abf.maxCommitSize(), 50);


    ASSERT_TRUE(abf.commit(75));            // rdp=25, wrp=25 (50+75 % 100), c=100
    EXPECT_EQ(abf.priWritePtr(0), wr1+25);
    EXPECT_EQ(abf.secWritePtr(0), nullptr);
    EXPECT_EQ(abf.priReadPtr(0), rd1+25);
    EXPECT_EQ(abf.secReadPtr(0), rd1);
    EXPECT_EQ(abf.maxCommitSize(), 100);


    ASSERT_TRUE(abf.release(75));           // rdp=0 (25+75 % 100), wrp=25, c=25
    EXPECT_EQ(abf.priWritePtr(0), wr1+25);
    EXPECT_EQ(abf.secWritePtr(0), nullptr);
    EXPECT_EQ(abf.priReadPtr(0), rd1);
    EXPECT_EQ(abf.secReadPtr(0), nullptr);
    EXPECT_EQ(abf.maxCommitSize(), 100);

    ASSERT_FALSE(abf.release(50));          // not possible (c=25 < 50)
    ASSERT_FALSE(abf.commit(100));          // not possible (c=25 + 100 > 100)
    EXPECT_EQ(abf.maxCommitSize(), 100);
}

TEST(AudioBuffer, inputWrapper) {
    juce::AudioBuffer<float> ab;
    ab.setSize(2, 100);
    AudioBufferInputWrapper abiw(&ab);

    EXPECT_EQ(abiw.noChannels(), 2);

    EXPECT_EQ(abiw.secWritePtr(0), nullptr);
    EXPECT_EQ(abiw.secWriteSize(), 0);

    EXPECT_EQ(abiw.priWritePtr(0), ab.getWritePointer(0));
    EXPECT_EQ(abiw.priWriteSize(), 100);

    ASSERT_TRUE(abiw.commit(50));

    EXPECT_EQ(abiw.priWritePtr(0), ab.getWritePointer(0, 50));
    EXPECT_EQ(abiw.priWriteSize(), 50);

    ASSERT_TRUE(abiw.commit(50));

    EXPECT_EQ(abiw.priWritePtr(0), nullptr);
    EXPECT_EQ(abiw.priWriteSize(), 0);
}


TEST(AudioBuffer, outputWrapper) {
    juce::AudioBuffer<float> ab;
    ab.setSize(2, 100);
    AudioBufferOutputWrapper abow(&ab);

    EXPECT_EQ(abow.noChannels(), 2);

    EXPECT_EQ(abow.secReadPtr(0), nullptr);
    EXPECT_EQ(abow.secReadSize(), 0);

    EXPECT_EQ(abow.priReadPtr(0), ab.getReadPointer(0));
    EXPECT_EQ(abow.priReadSize(), 100);

    ASSERT_TRUE(abow.release(50));

    EXPECT_EQ(abow.priReadPtr(0), ab.getReadPointer(0, 50));
    EXPECT_EQ(abow.priReadSize(), 50);

    ASSERT_TRUE(abow.release(50));

    EXPECT_EQ(abow.priReadPtr(0), nullptr);
    EXPECT_EQ(abow.priReadSize(), 0);
}
