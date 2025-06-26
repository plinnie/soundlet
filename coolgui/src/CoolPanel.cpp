/*
  ==============================================================================

    CoolPanel.cpp
    Created: 20 Dec 2022 1:15:43pm
    Author:  Vincent van Beveren

  ==============================================================================
*/

#include "CoolPanel.hpp"
#include "CoolPalette.hpp"

static const juce::Colour ColorBgStart = juce::Colour::fromRGB(240, 240, 240);
static const juce::Colour ColorBgEnd = juce::Colour::fromRGB(255, 255, 255);
static const juce::Colour ColorBorderOuter = juce::Colour::fromRGBA(192, 192, 192, 255);
static const juce::Colour ColorBorderMid = juce::Colour::fromRGBA(192, 192, 192, 120);
static const juce::Colour ColorBorderInner = juce::Colour::fromRGBA(192, 192, 192, 60);
static const juce::Colour ColorShimStart = juce::Colour::fromRGB(255, 255, 255);
static const juce::Colour ColorShimEnd = juce::Colour(0);

//==============================================================================
CoolPanel::CoolPanel(std::string title) : _title(title), _lastX(0), _lastY(0), _cached(nullptr)
{
}

CoolPanel::~CoolPanel()
{
    if (_cached) delete _cached;
    for (juce::Component* c : _labels)
    {
        delete c;
    }
}

void CoolPanel::paint (juce::Graphics& cg)
{
    if (!_cached) 
    {
        _cached = new juce::Image(juce::Image::RGB, getWidth(), getHeight(), false);
        juce::Graphics g(*_cached);
        g.setGradientFill(
            juce::ColourGradient::vertical(ColorBgStart, 0, ColorBgEnd, getHeight()));

        g.fillAll();   // clear the background

        g.setColour(ColorBorderOuter);
        g.drawRect(0, 0, getWidth(), getHeight(), 1);
        g.setColour(ColorBorderInner);
        g.drawRect(1, 1, getWidth() - 2, getHeight() - 2, 1);

        const int glowHPad = 4;
        const int glowStart = 20;
        
        int glowLength = getHeight() - glowStart;
        if (glowLength > 80) glowLength = 80;

        const int glowBorder = 3;
        const int FontOffsetX = 20;
        const int FontOffsetY = 5;
        const int LinePadding = 4;
        const int LineDescent = 4;

        g.setGradientFill(juce::ColourGradient::vertical(ColorShimStart, glowStart + glowBorder * 2, ColorShimEnd, glowStart + glowLength - glowBorder));


        g.fillRoundedRectangle(glowHPad, glowStart, getWidth() - glowHPad * 2, glowLength, glowBorder);

        g.setColour(darkBlue);
        g.drawText(_title, FontOffsetX, FontOffsetY, getWidth() - FontOffsetX * 2, getHeight(),
            juce::Justification::top, true);
        int w = g.getCurrentFont().getStringWidth(_title);
        g.fillRect(FontOffsetX + w + LinePadding, FontOffsetY + LineDescent, getWidth() - (FontOffsetX + w) - LinePadding * 2, 4);
        g.fillRect(LinePadding, FontOffsetY + LineDescent, FontOffsetX - LinePadding * 2, 4);
    }
    cg.drawImageAt(*_cached, 0, 0);
}

juce::Point<int> CoolPanel::gridToPoint(int x, int y, int tweakX, int tweakY)
{
    return juce::Point<int>(x * 16 + 8 + tweakX, y * 16 + 24 + tweakY);
}

void CoolPanel::addButton(CoolButton* cp, int x, int y, bool relative)
{
    addCool(cp, x, y, relative, BUTTON);
}

void CoolPanel::addKnob(CoolKnob* cp, int x, int y, bool relative)
{
    addCool(cp, x, y, relative, KNOB);
}

void CoolPanel::addComponent(Component * cp, int x, int y, bool relative)
{
    addCool(cp, x, y, relative, KNOB);
}

void CoolPanel::addCool(Component * cp, int x, int y, bool relative, CoolType ct)
{
    if (relative) 
    {
        x = _lastX + x;
        y = _lastY + y;
    }
    _lastX = x;
    _lastY = y;
    switch (ct) {
    case BUTTON:
        cp->setTopLeftPosition(gridToPoint(x, y, 4));
        break;
    default:
        cp->setTopLeftPosition(gridToPoint(x, y));
        break;
    }
    addAndMakeVisible(cp);
    if (cp->getTitle().isNotEmpty()) {
        juce::Label* lbl = new juce::Label();
        lbl->setText(cp->getTitle(), juce::NotificationType::dontSendNotification);
        lbl->setFont(12);
        switch (ct) {
        case BUTTON:
            lbl->setTopLeftPosition(gridToPoint(x + 2, y, 0, 1));
            lbl->setSize(64, 32);
            lbl->setJustificationType(juce::Justification::centredLeft);
            break;
        case KNOB:
            lbl->setTopLeftPosition(gridToPoint(x, y + 4, 0, -4));
            lbl->setSize(64, 16);
            lbl->setJustificationType(juce::Justification::centredTop);
            break;
        }
        addAndMakeVisible(lbl);
        _labels.push_back(lbl);
    }
}


void CoolPanel::resized()
{
    delete _cached;
    _cached = nullptr;
}
