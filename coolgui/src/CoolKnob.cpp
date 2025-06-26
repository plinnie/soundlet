/*
  ==============================================================================

    CoolKnob.cpp
    Created: 3 Jan 2023 3:23:03pm
    Author:  Vincent van Beveren

  ==============================================================================
*/

#include "CoolKnob.hpp"

//==============================================================================
CoolKnob::CoolKnob() : juce::Slider()
{
    setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
    setTextBoxStyle(juce::Slider::NoTextBox, false, 80, 20);
    setTextBoxIsEditable(false);
    setSize(64, 64);
}

CoolKnob::~CoolKnob()
{
}

void CoolKnob::mouseDown(const juce::MouseEvent& event)
{
    Slider::mouseDown(event);
    setMouseCursor(juce::MouseCursor::NoCursor);
    _mousePosition = juce::Desktop::getMousePosition();
}

void CoolKnob::mouseUp(const juce::MouseEvent& event)
{
    Slider::mouseUp(event);
    juce::Desktop::setMousePosition(_mousePosition);
    setMouseCursor(juce::MouseCursor::NormalCursor);
}

