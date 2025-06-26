#include "WaveletListBoxModel.hpp"

#include "BinaryData.h"
#include "wavelet_coeffs.hpp"

const auto Scaling = 0.7f;

WaveletListBoxModel::WaveletListBoxModel() {
    _waveletImages = juce::ImageCache::getFromMemory(
        BinaryData::wavelets_png, BinaryData::wavelets_pngSize);
    _rowHeight = _waveletImages.getHeight() / WAVELET_COUNT;
}

WaveletListBoxModel::~WaveletListBoxModel() {

}


int WaveletListBoxModel::getRowHeight() {
    return _rowHeight;
}

int WaveletListBoxModel::getWidth() {
    return _waveletImages.getWidth() ;
}


int WaveletListBoxModel::getNumRows() {
    return WAVELET_COUNT;
}

void WaveletListBoxModel::paintListBoxItem (int rowNumber,
                                   juce::Graphics& g,
                                   int width, int height,
                                   bool rowIsSelected) {
    if (rowNumber < 0 || rowNumber >= WAVELET_COUNT) return;

    g.drawImage(_waveletImages,
            0, 0,
           width, height,
            0, rowNumber * _rowHeight,
            _waveletImages.getWidth(), _rowHeight,
            false);
    if (rowIsSelected) {
        g.setColour(juce::Colours::white);
        g.drawRect(1, 1, width - 3, height - 3, 1);
    }

}
