/*
  ==============================================================================

    CoolButton.h
    Created: 4 Jan 2023 11:10:20am
    Author:  Vincent van Beveren

  ==============================================================================
*/

#pragma once

#include "juce_gui_basics/juce_gui_basics.h"

//==============================================================================
/*
*/
class CoolButton : public juce::ImageButton
{
public:
    CoolButton();
    ~CoolButton() override;


private:
    juce::Image _orig;


    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(CoolButton)
};
