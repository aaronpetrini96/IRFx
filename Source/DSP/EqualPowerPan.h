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
    // Clamp pan value
    pan = juce::jlimit(-1.0f, 1.0f, pan);

    // Convert pan value to angle between 45° (left) and 135° (right)
    const float angle = (juce::MathConstants<float>::pi * 0.25f) * (1.0f - pan); // Left: pi/2, Right: 0
    const float leftGain = std::sin(angle);
    const float rightGain = std::cos(angle);

    const int numSamples = buffer.getNumSamples();
    const int numChannels = buffer.getNumChannels();

    if (numChannels == 1)
    {
        // Expand to stereo
        juce::AudioBuffer<float> stereoBuffer(2, numSamples);
        stereoBuffer.clear();

        const float* mono = buffer.getReadPointer(0);

        stereoBuffer.copyFrom(0, 0, mono, numSamples); // Left
        stereoBuffer.copyFrom(1, 0, mono, numSamples); // Right

        stereoBuffer.applyGain(0, 0, numSamples, leftGain);
        stereoBuffer.applyGain(1, 0, numSamples, rightGain);

        buffer = std::move(stereoBuffer); // Replace original
    }
    else if (numChannels >= 2)
    {
        // Handle pseudo-mono: one channel has signal, one is silent
        const float leftLevel  = buffer.getRMSLevel(0, 0, numSamples);
        const float rightLevel = buffer.getRMSLevel(1, 0, numSamples);

        if (leftLevel > 0.0001f && rightLevel < 0.0001f)
        {
            buffer.copyFrom(1, 0, buffer, 0, 0, numSamples); // duplicate L to R
        }
        else if (rightLevel > 0.0001f && leftLevel < 0.0001f)
        {
            buffer.copyFrom(0, 0, buffer, 1, 0, numSamples); // duplicate R to L
        }

        buffer.applyGain(0, 0, numSamples, leftGain);
        buffer.applyGain(1, 0, numSamples, rightGain);
    }
}

