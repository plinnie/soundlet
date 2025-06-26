/*
  ==============================================================================

    CoolButton.h
    Created: 4 Jan 2023 11:10:20am
    Author:  Vincent van Beveren

  ==============================================================================
*/

#pragma once

#include <vector>
#include "juce_gui_basics/juce_gui_basics.h"

//==============================================================================
/*
*/
class CoolSlider : public juce::Slider
{
public:
    CoolSlider();
    ~CoolSlider() override;
    void setTicks(std::vector<float> ticks);
    std::vector<float> & getTicks();
private:
    std::vector<float> _ticks;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(CoolSlider)
};
