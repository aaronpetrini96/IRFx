/*
  ==============================================================================

    EqualPowerPan.h
    Created: 23 Jul 2025 8:37:46pm
    Author:  Aaron Petrini

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>

/**
 * Applies equal-power panning to a buffer.
 * Pan range: -1.0 (left) to +1.0 (right), 0.0 is center.
 * Expands mono to stereo if needed.
 */


inline void applyEqualPowerPan(juce::AudioBuffer<float>& buffer, float pan)
{
    {
        juce::dsp::AudioBlock<float> audioBlock (buffer);
        pan = juce::jlimit(-1.0f, 1.0f, pan);
        
        const float angle     = (juce::MathConstants<float>::halfPi * 0.5f) * (pan + 1.0f); // 0..π/2
        const float leftGain  = std::cos(angle);
        const float rightGain = std::sin(angle);
        
        auto left  = audioBlock.getChannelPointer(0);
        auto right = audioBlock.getChannelPointer(1);
        const int numSamples = (int)audioBlock.getNumSamples();
        
        juce::FloatVectorOperations::multiply(left, leftGain, numSamples);
        juce::FloatVectorOperations::multiply(right, rightGain, numSamples);
    }
}




