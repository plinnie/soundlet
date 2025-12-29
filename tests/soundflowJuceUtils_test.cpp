#include <catch2/catch_test_macros.hpp>
#include "soundflowJuceUtils.hpp"

TEST_CASE("AudioBufferFifo sizes", "[AudioBuffer]")
{
    AudioBufferFifo abf;
    abf.init(2, 100);

    REQUIRE(abf.noChannels() == 2);

    REQUIRE(abf.priWriteSize() == 100);
    REQUIRE(abf.secWriteSize() == 0);
    REQUIRE(abf.priReadSize()  == 0);
    REQUIRE(abf.secReadSize()  == 0);
    REQUIRE(abf.maxCommitSize() == 0);

    abf.commit(50);
    REQUIRE(abf.priWriteSize() == 50);
    REQUIRE(abf.secWriteSize() == 0);
    REQUIRE(abf.priReadSize()  == 50);
    REQUIRE(abf.secReadSize()  == 0);
    REQUIRE(abf.maxCommitSize() == 50);

    abf.release(25);
    REQUIRE(abf.priWriteSize() == 50);
    REQUIRE(abf.secWriteSize() == 25);
    REQUIRE(abf.priReadSize()  == 25);
    REQUIRE(abf.secReadSize()  == 0);
    REQUIRE(abf.maxCommitSize() == 50);

    abf.commit(75);
    REQUIRE(abf.priWriteSize() == 0);
    REQUIRE(abf.secWriteSize() == 0);
    REQUIRE(abf.priReadSize()  == 75);
    REQUIRE(abf.secReadSize()  == 25);
    REQUIRE(abf.maxCommitSize() == 100);

    abf.release(75);
    REQUIRE(abf.priWriteSize() == 75);
    REQUIRE(abf.secWriteSize() == 0);
    REQUIRE(abf.priReadSize()  == 25);
    REQUIRE(abf.secReadSize()  == 0);
    REQUIRE(abf.maxCommitSize() == 100);
}

TEST_CASE("AudioBufferFifo pointers", "[AudioBuffer]")
{
    AudioBufferFifo abf;
    abf.init(2, 100);

    // base pointers
    float*        wr1 = abf.priWritePtr(0);
    const float*  rd1 = abf.priReadPtr(0);

    REQUIRE(abf.commit(50));            // rdp=0, wrp=50, c=50
    REQUIRE(abf.priWritePtr(0) == wr1 + 50);
    REQUIRE(abf.secWritePtr(0) == nullptr);
    REQUIRE(abf.priReadPtr(0)  == rd1);
    REQUIRE(abf.secReadPtr(0)  == nullptr);
    REQUIRE(abf.maxCommitSize() == 50);

    REQUIRE(abf.release(25));           // rdp=25, wrp=50, c=25
    REQUIRE(abf.priWritePtr(0) == wr1 + 50);
    REQUIRE(abf.secWritePtr(0) == wr1);
    REQUIRE(abf.priReadPtr(0)  == rd1 + 25);
    REQUIRE(abf.secReadPtr(0)  == nullptr);
    REQUIRE(abf.maxCommitSize() == 50);

    REQUIRE(abf.commit(75));            // rdp=25, wrp=25, c=100
    REQUIRE(abf.priWritePtr(0) == wr1 + 25);
    REQUIRE(abf.secWritePtr(0) == nullptr);
    REQUIRE(abf.priReadPtr(0)  == rd1 + 25);
    REQUIRE(abf.secReadPtr(0)  == rd1);
    REQUIRE(abf.maxCommitSize() == 100);

    REQUIRE(abf.release(75));           // rdp=0, wrp=25, c=25
    REQUIRE(abf.priWritePtr(0) == wr1 + 25);
    REQUIRE(abf.secWritePtr(0) == nullptr);
    REQUIRE(abf.priReadPtr(0)  == rd1);
    REQUIRE(abf.secReadPtr(0)  == nullptr);
    REQUIRE(abf.maxCommitSize() == 100);

    REQUIRE_FALSE(abf.release(50));     // c=25 < 50
    REQUIRE_FALSE(abf.commit(100));     // c=25 + 100 > 100
    REQUIRE(abf.maxCommitSize() == 100);
}

TEST_CASE("AudioBuffer input wrapper", "[AudioBuffer]")
{
    juce::AudioBuffer<float> ab;
    ab.setSize(2, 100);

    AudioBufferInputWrapper abiw(&ab);

    REQUIRE(abiw.noChannels() == 2);

    REQUIRE(abiw.secWritePtr(0) == nullptr);
    REQUIRE(abiw.secWriteSize() == 0);

    REQUIRE(abiw.priWritePtr(0)  == ab.getWritePointer(0));
    REQUIRE(abiw.priWriteSize() == 100);

    REQUIRE(abiw.commit(50));
    REQUIRE(abiw.priWritePtr(0)  == ab.getWritePointer(0, 50));
    REQUIRE(abiw.priWriteSize() == 50);

    REQUIRE(abiw.commit(50));
    REQUIRE(abiw.priWritePtr(0)  == nullptr);
    REQUIRE(abiw.priWriteSize() == 0);
}

TEST_CASE("AudioBuffer output wrapper", "[AudioBuffer]")
{
    juce::AudioBuffer<float> ab;
    ab.setSize(2, 100);

    AudioBufferOutputWrapper abow(&ab);

    REQUIRE(abow.noChannels() == 2);

    REQUIRE(abow.secReadPtr(0) == nullptr);
    REQUIRE(abow.secReadSize() == 0);

    REQUIRE(abow.priReadPtr(0)  == ab.getReadPointer(0));
    REQUIRE(abow.priReadSize() == 100);

    REQUIRE(abow.release(50));
    REQUIRE(abow.priReadPtr(0)  == ab.getReadPointer(0, 50));
    REQUIRE(abow.priReadSize() == 50);

    REQUIRE(abow.release(50));
    REQUIRE(abow.priReadPtr(0)  == nullptr);
    REQUIRE(abow.priReadSize() == 0);
}
