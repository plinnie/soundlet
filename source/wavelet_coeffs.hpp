/*
  Wavelet coefficients header
  
  Kindly borrowed from pywavlets, thanks!
  
  https://github.com/PyWavelets/pywt
*/
#pragma once

struct WaveletCoeffs
{
    const char * const name;
    const unsigned int length;
    const float* dc_lo;
    const float* dc_hi;
    const float* rc_lo;
    const float* rc_hi; 
};

extern const WaveletCoeffs bior1_1_coeffs;
extern const WaveletCoeffs bior1_3_coeffs;
extern const WaveletCoeffs bior1_5_coeffs;
extern const WaveletCoeffs bior2_2_coeffs;
extern const WaveletCoeffs bior2_4_coeffs;
extern const WaveletCoeffs bior2_6_coeffs;
extern const WaveletCoeffs bior2_8_coeffs;
extern const WaveletCoeffs bior3_1_coeffs;
extern const WaveletCoeffs bior3_3_coeffs;
extern const WaveletCoeffs bior3_5_coeffs;
extern const WaveletCoeffs bior3_7_coeffs;
extern const WaveletCoeffs bior3_9_coeffs;
extern const WaveletCoeffs bior4_4_coeffs;
extern const WaveletCoeffs bior5_5_coeffs;
extern const WaveletCoeffs bior6_8_coeffs;
extern const WaveletCoeffs coif1_coeffs;
extern const WaveletCoeffs coif2_coeffs;
extern const WaveletCoeffs coif3_coeffs;
extern const WaveletCoeffs coif4_coeffs;
extern const WaveletCoeffs coif5_coeffs;
extern const WaveletCoeffs coif6_coeffs;
extern const WaveletCoeffs coif7_coeffs;
extern const WaveletCoeffs coif8_coeffs;
extern const WaveletCoeffs coif9_coeffs;
extern const WaveletCoeffs coif10_coeffs;
extern const WaveletCoeffs coif11_coeffs;
extern const WaveletCoeffs coif12_coeffs;
extern const WaveletCoeffs coif13_coeffs;
extern const WaveletCoeffs coif14_coeffs;
extern const WaveletCoeffs coif15_coeffs;
extern const WaveletCoeffs coif16_coeffs;
extern const WaveletCoeffs coif17_coeffs;
extern const WaveletCoeffs db1_coeffs;
extern const WaveletCoeffs db2_coeffs;
extern const WaveletCoeffs db3_coeffs;
extern const WaveletCoeffs db4_coeffs;
extern const WaveletCoeffs db5_coeffs;
extern const WaveletCoeffs db6_coeffs;
extern const WaveletCoeffs db7_coeffs;
extern const WaveletCoeffs db8_coeffs;
extern const WaveletCoeffs db9_coeffs;
extern const WaveletCoeffs db10_coeffs;
extern const WaveletCoeffs db11_coeffs;
extern const WaveletCoeffs db12_coeffs;
extern const WaveletCoeffs db13_coeffs;
extern const WaveletCoeffs db14_coeffs;
extern const WaveletCoeffs db15_coeffs;
extern const WaveletCoeffs db16_coeffs;
extern const WaveletCoeffs db17_coeffs;
extern const WaveletCoeffs db18_coeffs;
extern const WaveletCoeffs db19_coeffs;
extern const WaveletCoeffs db20_coeffs;
extern const WaveletCoeffs db21_coeffs;
extern const WaveletCoeffs db22_coeffs;
extern const WaveletCoeffs db23_coeffs;
extern const WaveletCoeffs db24_coeffs;
extern const WaveletCoeffs db25_coeffs;
extern const WaveletCoeffs db26_coeffs;
extern const WaveletCoeffs db27_coeffs;
extern const WaveletCoeffs db28_coeffs;
extern const WaveletCoeffs db29_coeffs;
extern const WaveletCoeffs db30_coeffs;
extern const WaveletCoeffs db31_coeffs;
extern const WaveletCoeffs db32_coeffs;
extern const WaveletCoeffs db33_coeffs;
extern const WaveletCoeffs db34_coeffs;
extern const WaveletCoeffs db35_coeffs;
extern const WaveletCoeffs db36_coeffs;
extern const WaveletCoeffs db37_coeffs;
extern const WaveletCoeffs db38_coeffs;
extern const WaveletCoeffs dmey_coeffs;
extern const WaveletCoeffs haar_coeffs;
extern const WaveletCoeffs rbio1_1_coeffs;
extern const WaveletCoeffs rbio1_3_coeffs;
extern const WaveletCoeffs rbio1_5_coeffs;
extern const WaveletCoeffs rbio2_2_coeffs;
extern const WaveletCoeffs rbio2_4_coeffs;
extern const WaveletCoeffs rbio2_6_coeffs;
extern const WaveletCoeffs rbio2_8_coeffs;
extern const WaveletCoeffs rbio3_1_coeffs;
extern const WaveletCoeffs rbio3_3_coeffs;
extern const WaveletCoeffs rbio3_5_coeffs;
extern const WaveletCoeffs rbio3_7_coeffs;
extern const WaveletCoeffs rbio3_9_coeffs;
extern const WaveletCoeffs rbio4_4_coeffs;
extern const WaveletCoeffs rbio5_5_coeffs;
extern const WaveletCoeffs rbio6_8_coeffs;
extern const WaveletCoeffs sym2_coeffs;
extern const WaveletCoeffs sym3_coeffs;
extern const WaveletCoeffs sym4_coeffs;
extern const WaveletCoeffs sym5_coeffs;
extern const WaveletCoeffs sym6_coeffs;
extern const WaveletCoeffs sym7_coeffs;
extern const WaveletCoeffs sym8_coeffs;
extern const WaveletCoeffs sym9_coeffs;
extern const WaveletCoeffs sym10_coeffs;
extern const WaveletCoeffs sym11_coeffs;
extern const WaveletCoeffs sym12_coeffs;
extern const WaveletCoeffs sym13_coeffs;
extern const WaveletCoeffs sym14_coeffs;
extern const WaveletCoeffs sym15_coeffs;
extern const WaveletCoeffs sym16_coeffs;
extern const WaveletCoeffs sym17_coeffs;
extern const WaveletCoeffs sym18_coeffs;
extern const WaveletCoeffs sym19_coeffs;
extern const WaveletCoeffs sym20_coeffs;

#define WAVELET_COUNT 106

extern const WaveletCoeffs * const wavelet_coeffs[106];
