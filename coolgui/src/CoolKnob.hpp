/*
  ==============================================================================

    CoolKnob.h
    Created: 3 Jan 2023 3:23:03pm
    Author:  Vincent van Beveren

  ==============================================================================
*/

#pragma once

#include "juce_gui_basics/juce_gui_basics.h"

//==============================================================================
/*
*/
class CoolKnob : public juce::Slider
{
public:
    CoolKnob();
    ~CoolKnob() override;

    void mouseDown(const juce::MouseEvent& event) override;
    void mouseUp(const juce::MouseEvent& event) override;

private:
    juce::Point<int> _mousePosition;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (CoolKnob)
};
