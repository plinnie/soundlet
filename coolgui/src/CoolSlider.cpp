#include "CoolSlider.hpp"


CoolSlider::CoolSlider() : juce::Slider() {
    setSliderStyle(juce::Slider::SliderStyle::LinearHorizontal);
    setTextBoxStyle(juce::Slider::NoTextBox, false, 80, 20);
    setTextBoxIsEditable(false);
    setSize(192, 32);
}

CoolSlider::~CoolSlider() {
}

void CoolSlider::setTicks(std::vector<float> ticks) {
    _ticks = ticks;
}

std::vector<float> & CoolSlider::getTicks() {
    return _ticks;
}
