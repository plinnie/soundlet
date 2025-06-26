#pragma once
#include "juce_gui_basics/juce_gui_basics.h"

#include "SoundletProcessor.hpp"
#include "CoolLookAndFeel.hpp"
#include "CoolKnob.hpp"
#include "CoolButton.hpp"
#include "CoolPanel.hpp"
#include "CoolSlider.hpp"

#include "WaveletListBoxModel.hpp"
#include "ListBoxParameterAttachment.hpp"

using SliderAttachment = juce::AudioProcessorValueTreeState::SliderAttachment;
using ButtonAttachment = juce::AudioProcessorValueTreeState::ButtonAttachment;

//==============================================================================
/**
*/
class SoundletEditor  : public juce::AudioProcessorEditor
{
public:
    SoundletEditor (SoundletProcessor&);
    ~SoundletEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    SoundletProcessor& audioProcessor;
    CoolLookAndFeel _laf;

    CoolPanel _gainPanel;

    juce::Label _detailLabel[MAX_DEPTH];
    CoolSlider _detailGain[MAX_DEPTH];
    std::unique_ptr<SliderAttachment> _detailAttach[MAX_DEPTH];
    juce::Label _lowApproxLabel;
    CoolSlider _lowApproxGain;
    std::unique_ptr<SliderAttachment> _lowApproxAttach;

    juce::Label _lowGain;
    juce::Label _zeroGain;
    juce::Label _hiGain;

    CoolPanel _waveletPanel;
    juce::ListBox _waveletListBox;
    WaveletListBoxModel _waveletModel;
    std::unique_ptr<ListBoxAttachment> _waveletAttach;

    CoolPanel _optionsPanel;
    CoolButton _syncFilters;
    std::unique_ptr<ButtonAttachment> _syncAttachment;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SoundletEditor)
};
