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
 * Pan range: -1.0 (full left) to +1.0 (full right), 0.0 is center.
 * - If the buffer is mono, it will be expanded to stereo.
 * - If the buffer is stereo, left and right gain will be applied directly.
 */
inline void applyEqualPowerPan(juce::AudioBuffer<float>& buffer, float pan)
{
    // Clamp pan
    pan = juce::jlimit(-1.0f, 1.0f, pan);

    // Convert pan to equal-power gains
    const float angle = 0.5f * juce::MathConstants<float>::pi * (pan + 1.0f);
    const float leftGain = std::cos(angle);
    const float rightGain = std::sin(angle);

    const int numSamples = buffer.getNumSamples();
    const int numChannels = buffer.getNumChannels();

    if (numChannels == 1)
    {
        // Expand mono to stereo
        juce::AudioBuffer<float> stereoBuffer(2, numSamples);
        stereoBuffer.clear();

        auto* monoData = buffer.getReadPointer(0);
        stereoBuffer.copyFrom(0, 0, monoData, numSamples); // Left
        stereoBuffer.copyFrom(1, 0, monoData, numSamples); // Right

        stereoBuffer.applyGain(0, 0, numSamples, leftGain);
        stereoBuffer.applyGain(1, 0, numSamples, rightGain);

        buffer.setSize(2, numSamples, false, false, true);
        buffer.copyFrom(0, 0, stereoBuffer.getReadPointer(0), numSamples);
        buffer.copyFrom(1, 0, stereoBuffer.getReadPointer(1), numSamples);
    }
    else if (numChannels >= 2)
    {
        buffer.applyGain(0, 0, numSamples, leftGain);
        buffer.applyGain(1, 0, numSamples, rightGain);
    }
}

