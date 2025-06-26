/*
  ==============================================================================

    CoolButton.cpp
    Created: 4 Jan 2023 11:10:20am
    Author:  Vincent van Beveren

  ==============================================================================
*/

#include "CoolButton.hpp"
#include "CoolGuiData.hpp"

//==============================================================================
CoolButton::CoolButton()
{
    _orig = juce::ImageCache::getFromMemory(CoolGuiData::switch_blue_modern_png,CoolGuiData::switch_blue_modern_pngSize);


    juce::Image up = _orig.getClippedImage(juce::Rectangle<int>(0, 0, 32, 32));
    juce::Image down = _orig.getClippedImage(juce::Rectangle<int>(0, 32, 32, 32));

    juce::Colour transp(juce::uint32(0));
    setClickingTogglesState(true);
    setImages(true, true, true,
        up, 1.0f, transp,
        up, 1.0f, transp,
        down, 1.0f, transp);
    setSize(32, 32);
}



CoolButton::~CoolButton()
{
}

