/*
  ==============================================================================

    CoolLookAndFeel.h
    Created: 3 Jan 2023 3:28:01pm
    Author:  Vincent van Beveren

  ==============================================================================
*/

#pragma once
#include "juce_gui_basics/juce_gui_basics.h"

#include "CoolPalette.hpp"

//==============================================================================
/*
*/
class CoolLookAndFeel : public juce::LookAndFeel_V4, public juce::DeletedAtShutdown
{
public:
    CoolLookAndFeel();
    ~CoolLookAndFeel() override;

    void drawRotarySlider(juce::Graphics& g, int x, int y, int width, int height, float sliderPos, float rotaryStartAngle, float rotaryEndAngle, juce::Slider& slider) override;
    void drawLinearSlider(juce::Graphics &  g, int x, int y, int width, int height, float sliderPos, float minSliderPos, float maxSliderPos, const juce::Slider::SliderStyle, juce::Slider &) override;

    JUCE_DECLARE_SINGLETON(CoolLookAndFeel, false);

    

private:
    juce::Image _knobImage;
    juce::Image _squareKnobImage;
    juce::Typeface::Ptr _typeface;
    
};



