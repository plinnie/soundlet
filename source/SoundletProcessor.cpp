#include "SoundletProcessor.hpp"

#include <assert.h>

#include <algorithm>

#include "SoundletEditor.hpp"

// A spacious estimate of the largest buffer
// ofcourse we could to better, but I'm too lazy to do the math
const auto MaxDetailBufSize = 16 * 1024;

//==============================================================================
SoundletProcessor::SoundletProcessor()
    : AudioProcessor(BusesProperties()
                         .withInput("Input", juce::AudioChannelSet::stereo(), true)
                         .withOutput("Output", juce::AudioChannelSet::stereo(), true)),
      _state(*this, nullptr, "state", createParameters())
    {
    _lowApproxGain = _state.getRawParameterValue("low_approx_gain");
    _state.addParameterListener("wavelet", this);
    _state.addParameterListener("sync_buffers", this);
    for (int i = 0; i < MAX_DEPTH; i++) {
        juce::String name = "detail_approx_gain_" + juce::String(i + 1);
        _detailGains[i] = _state.getRawParameterValue(name);
    }
}

juce::AudioProcessorValueTreeState::ParameterLayout SoundletProcessor::createParameters() {
    juce::AudioProcessorValueTreeState::ParameterLayout layout;

    juce::StringArray sa;

    for (int i = 0; i < WAVELET_COUNT; i++) {
        sa.add(wavelet_coeffs[i]->name);
    }

    layout.add(std::make_unique<juce::AudioParameterChoice>(juce::ParameterID{"wavelet", 1}, "Wavelet", sa, 0));

    layout.add(std::make_unique<juce::AudioParameterBool>(juce::ParameterID{"sync_buffers", 1}, "Sync buffers", true));

    for (int i = 0; i < MAX_DEPTH; i++) {
        juce::String name = "detail_approx_gain_" + juce::String(i + 1);
        juce::String label = "Detail gain " + juce::String(i + 1) + " [dB]";

        layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID{name, 1}, label, -40, 10, 0));
    }

    layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID{"low_approx_gain", 1},
                                                           "Low approx gain [dB]", -40, 10, 0));

    return layout;
}

SoundletProcessor::~SoundletProcessor() {}

//==============================================================================
const juce::String SoundletProcessor::getName() const { return JucePlugin_Name; }

bool SoundletProcessor::acceptsMidi() const { return false; }

bool SoundletProcessor::producesMidi() const { return false; }

bool SoundletProcessor::isMidiEffect() const { return false; }

double SoundletProcessor::getTailLengthSeconds() const { return 0.0; }

int SoundletProcessor::getNumPrograms() { return 1; }

int SoundletProcessor::getCurrentProgram() { return 0; }

void SoundletProcessor::setCurrentProgram(int index) { juce::ignoreUnused(index); }

const juce::String SoundletProcessor::getProgramName(int index) {
    juce::ignoreUnused(index);
    return {};
}

void SoundletProcessor::changeProgramName(int index, const juce::String& newName) {
    juce::ignoreUnused(index, newName);
}

void SoundletProcessor::parameterChanged(const juce::String& parameterID, float newValue) {
    if (parameterID == "wavelet" || parameterID == "sync_buffers") {
        _wavelet_changed = true;
    }
}

//==============================================================================

void SoundletProcessor::reconfigure(const WaveletCoeffs* coeffs, bool sync_buffers, int buffer_size) {
    if (_coeffs == coeffs && _sync_buffers == sync_buffers && buffer_size == _expectedBufSize) return;

    _expectedBufSize = buffer_size;

    bool diffParams;
    if (sync_buffers && _sync_buffers && _coeffs) {
        diffParams = (_coeffs->length != coeffs->length);
    } else {
        diffParams = true;
    }

    for (int i = 0; i < MAX_DEPTH; i++) {
        for (int j = 0; j < getNumInputChannels(); j++) {
            _banks[i][j].config(coeffs, diffParams, !sync_buffers);
        }
        if (diffParams) {
            // rough estimate of the maximum detail buffer size
            int detailBufSize = buffer_size / 2 + coeffs->length * 124;
            int rcBufSize = buffer_size;
            // make this sensible capacities.. somehow derive
            _detail[i].init(getTotalNumInputChannels(), (detailBufSize >> i)+64);

            _rc_bufs[i].init(getTotalNumInputChannels(), (rcBufSize >> i)+64);
        }
    }
    _coeffs = coeffs;
    _sync_buffers = sync_buffers;
}

void SoundletProcessor::prepareToPlay(double sampleRate, int samplesPerBlock) {
    juce::AudioParameterChoice* choice = static_cast<juce::AudioParameterChoice*>(_state.getParameter("wavelet"));
    bool sync_buffers = static_cast<juce::AudioParameterBool*>(_state.getParameter("sync_buffers"))->get();
    reconfigure(wavelet_coeffs[choice->getIndex()], sync_buffers, samplesPerBlock);
}

void SoundletProcessor::releaseResources() {}

bool SoundletProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const {
    if (layouts.getMainInputChannels() > MAX_CHANNELS) return false;
    if (layouts.getMainInputChannels() != layouts.getMainOutputChannels()) return false;
    return true;
}

void SoundletProcessor::updateAmplitudeTracking(juce::AudioBuffer<float>& buffer) {}

void SoundletProcessor::processLevelDecompose(sf::SoundOut& samples_in, sf::SoundIn& detail_out,
                                              sf::SoundIn& approx_out, int depth) {
    unsigned int released = 0, committed = 0;

    for (int ch = 0; ch < samples_in.noChannels(); ch++) {
        _banks[depth][ch].decompose(ch, samples_in, detail_out, approx_out, released, committed);
    }

    samples_in.release(released);

    approx_out.commit(committed);

    detail_out.commit(committed);
}

void SoundletProcessor::processLevelReconstruct(sf::SoundOut& detail_in, sf::SoundOut& approx_in,
                                                sf::SoundIn& samples_out, int depth) {
    unsigned int released = 0, committed = 0;
    for (int ch = 0; ch < approx_in.noChannels(); ch++) {
        _banks[depth][ch].reconstruct(ch, detail_in, approx_in, samples_out, released, committed,
                                      juce::Decibels::decibelsToGain<float>(*_detailGains[depth]));
    }
    detail_in.release(released);

    approx_in.release(released);

    samples_out.commit(committed);
}

void SoundletProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages) {
    unsigned int chs = getTotalNumInputChannels();
    unsigned int bufsize = buffer.getNumSamples();

    // Audacity has the audacity to lie to us, and provide more samples than they said
    // during prepareToPlay. As such, we'll need to check the provided buffer size and
    // re-init if needed. Thus extra check bufdize > _expectedBufSize
    if (_wavelet_changed || bufsize > _expectedBufSize) {
        juce::AudioParameterChoice* choice = static_cast<juce::AudioParameterChoice*>(_state.getParameter("wavelet"));
        bool sync_buffers = static_cast<juce::AudioParameterBool*>(_state.getParameter("sync_buffers"))->get();
        reconfigure(wavelet_coeffs[choice->getIndex()], sync_buffers, bufsize);
        _wavelet_changed = false;
    }

    AudioBufferOutputWrapper samples_in(&buffer);

    AudioBufferFifo approx;
    approx.init(samples_in.noChannels(), samples_in.readAvail());

    processLevelDecompose(samples_in, _detail[0], approx, 0);
    for (int depth = 1; depth < MAX_DEPTH; depth++) {
        processLevelDecompose(approx, _detail[depth], approx, depth);
    }

    buffer.applyGain(juce::Decibels::decibelsToGain<float>(*_lowApproxGain));
    processLevelReconstruct(_detail[MAX_DEPTH - 1], approx, _rc_bufs[MAX_DEPTH - 1], MAX_DEPTH - 1);
    for (int depth = MAX_DEPTH - 2; depth >= 0; depth--) {
        processLevelReconstruct(_detail[depth], _rc_bufs[depth + 1], _rc_bufs[depth], depth);
    }

    unsigned int available = _rc_bufs[0].readAvail();
    unsigned int consumed = 0;
    unsigned int offset;
    if (available == 0) 
    {
        buffer.clear();
        return;
    }
    
    if (available < bufsize) {
        offset = bufsize - available;
    } else {
        offset = 0;
    }
    consumed = consumeToAudioBuffer(buffer, offset, _rc_bufs[0], available);

    // some wavelets may create high amplitude atifacts.
    // clip at +10 dB just to be sure!
    for (unsigned int ch = 0; ch < chs; ch++) {
        juce::FloatVectorOperationsBase<float, int>::clip(buffer.getWritePointer(ch), buffer.getReadPointer(ch), -10.0f,
                                                          10.0f, bufsize);
    }

    // was the output buffer not fully filled? We move it to the end
    // and clear the beginning
    if (offset > 0) {
        buffer.clear(0, offset);
    }
}

//==============================================================================
bool SoundletProcessor::hasEditor() const {
    return true;  // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* SoundletProcessor::createEditor() {
    return new SoundletEditor(*this);
    ;
}

//==============================================================================
void SoundletProcessor::getStateInformation(juce::MemoryBlock& destData) {
    auto state = _state.copyState();
    std::unique_ptr<juce::XmlElement> xml(state.createXml());
    copyXmlToBinary(*xml, destData);
}

void SoundletProcessor::setStateInformation(const void* data, int sizeInBytes) {
    std::unique_ptr<juce::XmlElement> xmlState(getXmlFromBinary(data, sizeInBytes));

    if (xmlState.get() != nullptr)
        if (xmlState->hasTagName(_state.state.getType())) _state.replaceState(juce::ValueTree::fromXml(*xmlState));
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter() { return new SoundletProcessor(); }
