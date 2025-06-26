#pragma once
/**
 * @brief WaveletListBoxModel is a listbox model containing wavelets. Yes.
 * 
 */

#include "juce_gui_basics/juce_gui_basics.h"

class WaveletListBoxModel : public juce::ListBoxModel
{
public:
    WaveletListBoxModel();
    ~WaveletListBoxModel() override;
    int getNumRows() override;
    void paintListBoxItem (int rowNumber,
                                   juce::Graphics& g,
                                   int width, int height,
                                   bool rowIsSelected) override;
    int getRowHeight();
    int getWidth();

private:
    juce::Image _waveletImages;
    int _rowHeight;
};