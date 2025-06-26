/**
 * @file    WaveletFilter.hpp
 * @author  Vincent van Beveren (plinnie@protonmail.com)
 * @brief   Implementation of a wavelet decomposition / reconsturction
 * @date 2023-08-11
 *
 * @copyright Copyright (c) 2023 Vincent van Beveren
 */
#pragma once

#include "wavelet_coeffs.hpp"
#include "soundflow.hpp"

class RRConvBuffer {
    float *_buf = nullptr;
    unsigned int _size = 0;
    unsigned int _offset = 0;
    int _sample_count = 0;

   public:
    void config(unsigned int size, unsigned int wait = 2) {
        if (_size != size) {
            if (_buf) delete _buf;
            _buf = new float[size]();
        }
        _size = size;
        _offset = 0;
        _sample_count = -1 * ((int)wait);
    }

    inline void add(float sample) {
        _buf[_offset] = sample;
        _offset = _offset == _size - 1 ? 0 : _offset + 1;
        _sample_count = _sample_count == (unsigned int)(_size - 1) ? 0 : _sample_count + 1;
    }

    /**
     * @brief Calculate the number of output samples for a given number of input samples
     *
     * @param input_samples
     * @return unsigned int
     */
    unsigned int output_rc(unsigned int input_samples) { return 0; }

    inline bool is_ready_dc() { return _sample_count >= 0 && (_sample_count & 1) == 0; }

    inline bool is_ready_rc() { return _sample_count >= 0; }

    float &operator[](int index) { return _buf[(_offset + index) % _size]; }

    inline float conv(const float *kernel) {
        double sum = 0;
        float *ptr = &_buf[_offset];

        do {
            ptr--;
            if (ptr < &_buf[0]) ptr = &_buf[_size - 1];
            double x = *ptr;
            double y = *kernel++;
            //            printf("%1.12f x %1.12f\n", x, y);
            sum += x * y;
        } while (ptr != &_buf[_offset]);
        return sum;
    }

    ~RRConvBuffer() {
        if (_buf) delete _buf;
    }
};

class FilterBank {
   private:
    const WaveletCoeffs *_coeffs;
    RRConvBuffer _dc_buffer;
    RRConvBuffer _rc_detail;
    RRConvBuffer _rc_approx;
    int _kernel_size = 0;

   public:
    FilterBank() : _coeffs(nullptr) {}

    void config(const WaveletCoeffs *coeffs, bool reset = false, bool no_latency = false) {
        if (!reset && _coeffs && _coeffs->length == coeffs->length) {
            // Simple replacement. No buffer reinitialization required
            _coeffs = coeffs;
        } else {
            _coeffs = coeffs;
            _kernel_size = _coeffs->length;
            _dc_buffer.config(_kernel_size);
            if (no_latency) {
                _rc_detail.config(_kernel_size, 0);
                _rc_approx.config(_kernel_size, 0);
            } else {
                _rc_detail.config(_kernel_size, _kernel_size - 1);
                _rc_approx.config(_kernel_size, _kernel_size - 1);
            }
        }
    }

    void decompose(const float *in, unsigned int in_count, float *detail_out, float *approx_out,
                   unsigned int &out_count) {
        out_count = 0;
        for (unsigned int i = 0; i < in_count; i++) {
            _dc_buffer.add(in[i]);
            if (_dc_buffer.is_ready_dc()) {
                *detail_out++ = _dc_buffer.conv(_coeffs->dc_hi);
                *approx_out++ = _dc_buffer.conv(_coeffs->dc_lo);
                out_count++;
            }
        }
    }

    void decompose(int ch, sf::SoundOut &samples_in, sf::SoundIn &detail_out, sf::SoundIn &approx_out,
                   unsigned int &released, unsigned int &committed) {
        released = samples_in.readAvail();

        committed = 0;
        for (unsigned int i = 0; i < released; i++) {
            _dc_buffer.add(samples_in.read(ch, i));
            if (_dc_buffer.is_ready_dc()) {
                detail_out.write(ch, committed, _dc_buffer.conv(_coeffs->dc_hi));
                approx_out.write(ch, committed, _dc_buffer.conv(_coeffs->dc_lo));
                committed++;
            }
        }
    }

    void reconstruct(const float *detail_in, const float *approx_in, unsigned int in_count, float *samples_out,
                     unsigned int *out_count, float detailGain = 1.0f) {
        *out_count = 0;
        for (int i = 0; i < in_count; i++) {
            _rc_detail.add(detail_in[i]);
            _rc_approx.add(approx_in[i]);
            if (_rc_detail.is_ready_rc()) {
                *samples_out = _rc_detail.conv(_coeffs->rc_hi) * detailGain;
                *samples_out += _rc_approx.conv(_coeffs->rc_lo);
                samples_out++;
                (*out_count)++;
            }
            _rc_detail.add(0);
            _rc_approx.add(0);
            if (_rc_detail.is_ready_rc()) {
                *samples_out = _rc_detail.conv(_coeffs->rc_hi) * detailGain;
                *samples_out += _rc_approx.conv(_coeffs->rc_lo);
                samples_out++;
                (*out_count)++;
            }
        }
    }

    void reconstruct(int ch, sf::SoundOut &detail_in, sf::SoundOut &approx_in, sf::SoundIn &samples_out,
                     unsigned int &released, unsigned int &committed, float detailGain = 1.0f) {
        float sample;
        released = std::min(detail_in.readAvail(), approx_in.readAvail());
        committed = 0;
        for (int i = 0; i < released; i++) {
            _rc_detail.add(detail_in.read(ch, i));
            _rc_approx.add(approx_in.read(ch, i));
            if (_rc_detail.is_ready_rc()) {
                sample = _rc_detail.conv(_coeffs->rc_hi) * detailGain;
                sample += _rc_approx.conv(_coeffs->rc_lo);
                samples_out.write(ch, committed, sample);
                committed++;
            }
            _rc_detail.add(0);
            _rc_approx.add(0);
            if (_rc_detail.is_ready_rc()) {
                sample = _rc_detail.conv(_coeffs->rc_hi) * detailGain;
                sample += _rc_approx.conv(_coeffs->rc_lo);
                samples_out.write(ch, committed, sample);
                committed++;
            }
        }
    }
};
