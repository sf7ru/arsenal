// -*- tab-width: 4; Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil -*-

/****************************************************************************
 *
 *   Copyright (C) 2012 PX4 Development Team. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 * 3. Neither the name PX4 nor the names of its contributors may be
 *    used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 ****************************************************************************/

/// @file	LowPassFilter.h
/// @brief	A class to implement a second order low pass filter 
/// Author: Leonard Hall <LeonardTHall@gmail.com>
/// Adapted for PX4 by Andrew Tridgell

#pragma once

#ifndef ML_FLOAT
#define ML_FLOAT float
#endif

namespace math
{
class LowPassFilter2p
{
public:
    // constructor
    LowPassFilter2p(ML_FLOAT    sample_freq, ML_FLOAT    cutoff_freq) :
        _cutoff_freq(cutoff_freq),
        _a1(0.0f),
        _a2(0.0f),
        _b0(0.0f),
        _b1(0.0f),
        _b2(0.0f),
        _delay_element_1(0.0f),
        _delay_element_2(0.0f)
    {
        // set initial parameters
        set_cutoff_frequency(sample_freq, cutoff_freq);
    }

    /**
     * Change filter parameters
     */
    void set_cutoff_frequency(ML_FLOAT    sample_freq, ML_FLOAT    cutoff_freq);

    /**
     * Add a new raw value to the filter
     *
     * @return retrieve the filtered result
     */
        ML_FLOAT    apply(ML_FLOAT    sample);

    /**
     * Return the cutoff frequency
     */
        ML_FLOAT    get_cutoff_freq(void) const {
        return _cutoff_freq;
    }

    /**
     * Reset the filter state to this value
     */
    float reset(float sample);

private:
        ML_FLOAT    _cutoff_freq;
        ML_FLOAT    _a1;
        ML_FLOAT    _a2;
        ML_FLOAT    _b0;
        ML_FLOAT    _b1;
        ML_FLOAT    _b2;
        ML_FLOAT    _delay_element_1;        // buffered sample -1
        ML_FLOAT    _delay_element_2;        // buffered sample -2
};

} // namespace math
