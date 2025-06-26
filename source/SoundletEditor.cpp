/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "SoundletEditor.hpp"

#include <vector>
#include "SoundletProcessor.hpp"

const auto WaveletListHeight = 280;
const auto GainWidth = 285;
const auto GainHeight = 340;

//==============================================================================
SoundletEditor::SoundletEditor(SoundletProcessor& p)
    : AudioProcessorEditor(&p), audioProcessor(p), 
    _gainPanel("Gain"), _waveletPanel("Wavelet"), _optionsPanel("Options")
{
    juce::LookAndFeel::setDefaultLookAndFeel(CoolLookAndFeel::getInstance());

    
    const auto WaveletListWidth = _waveletModel.getWidth()+24;
    setSize (WaveletListWidth+GainWidth, GainHeight);
    _gainPanel.setBounds(0, 0, GainWidth, GainHeight);
    _waveletPanel.setBounds(GainWidth, 0, WaveletListWidth, WaveletListHeight);
    _optionsPanel.setBounds(GainWidth, WaveletListHeight, WaveletListWidth, GainHeight - WaveletListHeight);
    addAndMakeVisible(_gainPanel);
    addAndMakeVisible(_waveletPanel);
    addAndMakeVisible(_optionsPanel);

    juce::String fR;
    std::vector<float> ticks { -40.f, -30.f, -20.f, -10.f, 0.f, 10.f }; 
    
    float max_f = p.getSampleRate() / 2;

    for (int i = 0; i <MAX_DEPTH; ++i)
    {
        
        float center_f = max_f * 0.75;
        
        fR = juce::String::formatted("%.1f Hz", center_f); 
        max_f = max_f / 2;

        _detailLabel[i].setText(fR, juce::NotificationType::dontSendNotification);
        _detailLabel[i].setFont(_detailLabel[i].getFont().withHeight(10));
        _detailLabel[i].setSize(64, 32);
        _detailLabel[i].setJustificationType(juce::Justification::centredRight);
        _gainPanel.addComponent(&_detailLabel[i], i == 0 ? 0 : -4, i == 0 ? 0 : 2, true);

        juce::String name = "detail_approx_gain_" + juce::String(i + 1);
        _detailGain[i].setTicks(ticks);
        _detailAttach[i] = std::move(std::make_unique<SliderAttachment>(p.getState(), name, _detailGain[i]));
        _detailGain[i].setSize(216,32);
        _gainPanel.addComponent(&_detailGain[i], 4, 0, true);
    }


    fR = juce::String::formatted("%.1f Hz", max_f*0.75); 
    _lowApproxLabel.setText(fR, juce::NotificationType::dontSendNotification);
    _lowApproxLabel.setFont(_lowApproxLabel.getFont().withHeight(10));
    _lowApproxLabel.setSize(64, 32);
    _lowApproxLabel.setJustificationType(juce::Justification::centredRight);
    _gainPanel.addComponent(&_lowApproxLabel, -4, 2, true);
    _lowApproxGain.setSize(216,32);
    _lowApproxGain.setTicks(ticks);
    _lowApproxAttach = std::move(std::make_unique<SliderAttachment>(p.getState(), "low_approx_gain", _lowApproxGain));
    _gainPanel.addComponent(&_lowApproxGain, 4, 0, true);

    _lowGain.setText("-40 dB", juce::NotificationType::dontSendNotification);
    _lowGain.setFont(_lowGain.getFont().withHeight(10));
    _lowGain.setSize(32, 16);
    _lowGain.setJustificationType(juce::Justification::centredLeft);
    _gainPanel.addComponent(&_lowGain, 0, 2, true);

    _zeroGain.setText("0 dB", juce::NotificationType::dontSendNotification);
    _zeroGain.setFont(_zeroGain.getFont().withHeight(10));
    _zeroGain.setSize(32, 16);
    _gainPanel.addComponent(&_zeroGain, 8, 0, true);


    _hiGain.setText("10 dB", juce::NotificationType::dontSendNotification);
    _hiGain.setFont(_hiGain.getFont().withHeight(10));
    _hiGain.setSize(32, 16);
    _gainPanel.addComponent(&_hiGain, 2, 0, true);
    
    // _zeroGain;
    // _hiGain;


    _gainPanel.lookAndFeelChanged();

    _waveletListBox.setOutlineThickness(1);
    _waveletListBox.setModel(&_waveletModel);
    _waveletListBox.setSize(_waveletModel.getWidth()+10, WaveletListHeight-32);
    _waveletListBox.setRowHeight(_waveletModel.getRowHeight());
    _waveletListBox.selectRow(0, true, true);
    _waveletPanel.addComponent(&_waveletListBox, 0, 0, false);
    _waveletAttach = std::move(std::make_unique<ListBoxAttachment>(p.getState(), "wavelet", _waveletListBox));

    _syncFilters.setTitle("Sync filters");
    _syncAttachment = std::move(std::make_unique<ButtonAttachment>(p.getState(), "sync_buffers", _syncFilters));
    _optionsPanel.addButton(&_syncFilters, 0, 0);
}

SoundletEditor::~SoundletEditor()
{
    setLookAndFeel(nullptr);
}

//==============================================================================
void SoundletEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));
}

void SoundletEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
}
