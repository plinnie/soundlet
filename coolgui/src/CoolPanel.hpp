/*
  ==============================================================================

    CoolPanel.h
    Created: 20 Dec 2022 1:15:43pm
    Author:  Vincent van Beveren

  ==============================================================================
*/

#pragma once

#include <string>
#include <vector>

#include "juce_gui_basics/juce_gui_basics.h"

#include "CoolButton.hpp"
#include "CoolKnob.hpp"

enum CoolType { BUTTON, KNOB };

//==============================================================================
/*
*/
class CoolPanel  : public juce::Component
{
public:
    CoolPanel(std::string title);
    ~CoolPanel() override;

    void paint (juce::Graphics&) override;
    void resized() override;

    void addButton(CoolButton * cp, int x, int y, bool relative = false);
    void addKnob(CoolKnob * cp, int x, int y, bool relative = false);
    void addComponent(Component * cp, int x, int y, bool relative = false);


private:
    std::string _title;
    std::vector<Component*> _labels;
    juce::Image* _cached;
    int _lastX;
    int _lastY;
    void addCool(juce::Component* cp, int x, int y, bool relative, CoolType ct);
    juce::Point<int> gridToPoint(int x, int y, int tweakX = 0, int tweakY = 0);
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (CoolPanel)
};
