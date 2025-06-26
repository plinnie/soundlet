/*
  ==============================================================================

    CoolLookAndFeel.cpp
    Created: 3 Jan 2023 3:28:01pm
    Author:  Vincent van Beveren

  ==============================================================================
*/


#include "CoolLookAndFeel.hpp"
#include "CoolKnob.hpp"
#include "CoolSlider.hpp"
#include "CoolGuiData.hpp"


//==============================================================================
CoolLookAndFeel::CoolLookAndFeel() : LookAndFeel_V4()
{
    _knobImage = juce::ImageCache::getFromMemory(
        CoolGuiData::rotational_blue_modern_png, CoolGuiData::rotational_blue_modern_pngSize);
    _squareKnobImage = juce::ImageCache::getFromMemory(
        CoolGuiData::square_knob_modern_png, CoolGuiData::square_knob_modern_pngSize);
    _typeface = juce::Typeface::createSystemTypefaceFor(CoolGuiData::PastiRegular_otf, CoolGuiData::PastiRegular_otfSize);
    setDefaultSansSerifTypeface(_typeface);
    setDefaultSansSerifTypefaceName("PASTI");
    setColour(juce::ResizableWindow::backgroundColourId, juce::Colours::white);
    setColour(juce::Label::textColourId, juce::Colour(0xff0080c4));
    setColour(juce::Label::backgroundColourId, juce::Colour(0x0));
    setColour(juce::Label::outlineColourId, juce::Colour(0x0));
}

CoolLookAndFeel::~CoolLookAndFeel()
{
    clearSingletonInstance();
}

void CoolLookAndFeel::drawLinearSlider(juce::Graphics &  g, int x, int y, int width, int height, 
    float sliderPos, float minSliderPos, float maxSliderPos, 
    const juce::Slider::SliderStyle style, juce::Slider & slider)
{
    CoolSlider * cs = dynamic_cast<CoolSlider *>(&slider);

    if (!cs) {
        LookAndFeel_V4::drawLinearSlider(g, x, y, width, height, sliderPos, minSliderPos, maxSliderPos, style, slider);
        return;
    }

    // // we only do horizontal for now
    // const auto grooveHeight = 8;
    
    // int c = height / 2 - grooveHeight / 2;
    // g.setGradientFill(
    //     juce::ColourGradient::vertical(juce::Colours::black, 
    //         c, darkBlue, c + grooveHeight));

    // g.fillRoundedRectangle(0, c, width, grooveHeight, grooveHeight / 3);
    // g.setColour(juce::Colours::black);
    // g.drawRoundedRectangle(0, c, width, grooveHeight, grooveHeight / 3, 1);

    // juce::Range minMax = slider.getRange();
    int imgWidth = _squareKnobImage.getWidth();
    int imgHeight = _squareKnobImage.getHeight() / 2;
    // int xPos = ((sliderPos - minMax.getStart()) * width) / minMax.getLength();

    

    if (cs->isBar())
    {
        g.setColour (cs->findColour (juce::Slider::trackColourId));
        g.fillRect (cs->isHorizontal() ? juce::Rectangle<float> (static_cast<float> (x), (float) y + 0.5f, sliderPos - (float) x, (float) height - 1.0f)
                                          : juce::Rectangle<float> ((float) x + 0.5f, sliderPos, (float) width - 1.0f, (float) y + ((float) height - sliderPos)));

        drawLinearSliderOutline (g, x, y, width, height, style, slider);
    }
    else
    {
        auto trackWidth = juce::jmin (6.0f, cs->isHorizontal() ? (float) height * 0.25f : (float) width * 0.25f);

        juce::Point<float> startPoint (cs->isHorizontal() ? (float) x : (float) x + (float) width * 0.5f,
                                 cs->isHorizontal() ? (float) y + (float) height * 0.5f : (float) (height + y));

        juce::Point<float> endPoint (cs->isHorizontal() ? (float) (width + x) : startPoint.x,
                               cs->isHorizontal() ? startPoint.y : (float) y);


        g.setColour(lightBlue);
        for (float v : cs->getTicks())
        {
            juce::Range<double> range = cs->getRange();
            float r = (float) (v - range.getStart()) / range.getLength();
            int x = (int)(startPoint.getX() + r * ( endPoint.getX() - startPoint.getX()) + 0.5f);
            g.drawLine(x, 23, x, 30); 
        }


        juce::Path backgroundTrack;
        backgroundTrack.startNewSubPath (startPoint);
        backgroundTrack.lineTo (endPoint);

        g.setColour (cs->findColour (juce::Slider::backgroundColourId));
        g.strokePath (backgroundTrack, { trackWidth, juce::PathStrokeType::curved, juce::PathStrokeType::rounded });

        juce::Path valueTrack;
        juce::Point<float> minPoint, maxPoint, thumbPoint;

        auto kx = cs->isHorizontal() ? sliderPos : ((float) x + (float) width * 0.5f);
        auto ky = cs->isHorizontal() ? ((float) y + (float) height * 0.5f) : sliderPos;


        minPoint = startPoint;
        maxPoint = { kx, ky };

        valueTrack.startNewSubPath (minPoint);
        valueTrack.lineTo (maxPoint);
        g.setColour (cs->findColour (juce::Slider::trackColourId));
        g.strokePath (valueTrack, { trackWidth, juce::PathStrokeType::curved, juce::PathStrokeType::rounded });

        g.setColour (cs->findColour (juce::Slider::thumbColourId));

        juce::Rectangle<float> drawTo(static_cast<float>(height),static_cast<float>(height));
        drawTo = drawTo.withCentre(maxPoint);
        
        g.drawImage(_squareKnobImage, static_cast<int>(drawTo.getX()), static_cast<int>(drawTo.getY()), 
            static_cast<int>(drawTo.getWidth()), static_cast<int>(drawTo.getHeight()), 
            0, imgHeight, imgWidth, imgHeight);

    }

} 	

void CoolLookAndFeel::drawRotarySlider(juce::Graphics& g, int x, int y, int width, int height, float sliderPos, float rotaryStartAngle, float rotaryEndAngle, juce::Slider& slider)
{
    if (_knobImage.isValid())
    {
        const double rotation = (slider.getValue()
            - slider.getMinimum())
            / (slider.getMaximum()
                - slider.getMinimum());

        const int frames = 127;
        const int frameId = (int)ceil(rotation * ((double)frames - 1.0));
        const float radius = juce::jmin(width / 1.0f, height / 1.0f);

        int imgWidth = _knobImage.getWidth();
        int imgHeight = _knobImage.getHeight() / frames;
        g.drawImage(_knobImage, 0, 0, 64, 64, 0, frameId * imgHeight, imgWidth, imgHeight);
    }
    else
    {
        static const float textPpercent = 0.25f;
        juce::Rectangle<float> text_bounds(1.0f + width * (1.0f - textPpercent) / 2.0f, 0.5f * height, width * textPpercent, 0.5f * height);

        g.setColour(juce::Colours::black);

        g.drawFittedText(juce::String("No Image"), text_bounds.getSmallestIntegerContainer(), juce::Justification::horizontallyCentred | juce::Justification::centred, 1);
    }
}


JUCE_IMPLEMENT_SINGLETON(CoolLookAndFeel)