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


//// Prepare a stereo block for panning
//inline juce::dsp::AudioBlock<float> preparePanBlock(juce::AudioBuffer<float>& buffer, int numInputChannels)
//{
//    const auto numSamples = buffer.getNumSamples();
//
//    if (numInputChannels == 1)
//    {
//        // Make sure buffer has 2 channels (real-time safe: don't call setSize() here)
//        jassert(buffer.getNumChannels() >= 2);
//
//        auto* mono  = buffer.getReadPointer(0);
//        auto* right = buffer.getWritePointer(1);
//
//        // Duplicate mono → right
//        juce::FloatVectorOperations::copy(right, mono, numSamples);
//    }
//    else if (numInputChannels == 2)
//    {
//        // If right channel is silent, copy left channel to right (pseudo-mono detection)
//        auto* left = buffer.getReadPointer(0);
//        auto* right = buffer.getWritePointer(1);
//        
//        float rightLevel = buffer.getRMSLevel(1, 0, numSamples);
//        
//        if (rightLevel < 0.0001f)
//        {
//            juce::FloatVectorOperations::copy(right, left, numSamples);
//        }
//    }
//
//    // Always return stereo block
//    return juce::dsp::AudioBlock<float>(buffer).getSubsetChannelBlock(0, 2);
//}
//
//// Apply equal-power pan to stereo block
//inline void applyEqualPowerPan(juce::AudioBuffer<float>& buffer, float pan, int numInputChannels)
//{
//    juce::dsp::AudioBlock block = preparePanBlock(buffer, numInputChannels);
//    pan = juce::jlimit(-1.0f, 1.0f, pan);
//
//    const float angle     = (juce::MathConstants<float>::halfPi * 0.5f) * (pan + 1.0f); // 0..π/2
//    const float leftGain  = std::cos(angle);
//    const float rightGain = std::sin(angle);
//
//    auto* left  = block.getChannelPointer(0);
//    auto* right = block.getChannelPointer(1);
//
//    const int numSamples = (int)block.getNumSamples();
//
//    juce::FloatVectorOperations::multiply(left,  leftGain,  numSamples);
//    juce::FloatVectorOperations::multiply(right, rightGain, numSamples);
//}
//



//======= OLD CODE


inline void applyEqualPowerPan(juce::AudioBuffer<float>& buffer, float pan, int numInputChannels)
{
    // Clamp pan to [-1, 1]
    pan = juce::jlimit(-1.0f, 1.0f, pan);

    // Equal-power pan mapping: pan=-1 (L), pan=0 (center), pan=1 (R)
    const float angle     = (juce::MathConstants<float>::halfPi * 0.5f) * (pan + 1.0f); // 0..π/2
    const float leftGain  = std::cos(angle);
    const float rightGain = std::sin(angle);

    const int numSamples = buffer.getNumSamples();

    // If the input was mono, expand to stereo before panning
    if (numInputChannels == 1)
    {

        juce::AudioBuffer<float> monoBuffer(1, numSamples);
        
//        const float* left = buffer.getReadPointer(0);
//        const float* right = buffer.getNumChannels() > 1 ? buffer.getReadPointer(1) : nullptr;
//        float* mono = monoBuffer.getWritePointer(0);
        
        monoBuffer.copyFrom(0, 0, buffer, 0, 0, numSamples);
        
        // Now expand mono to stereoBuffer
//        juce::AudioBuffer<float> stereoBuffer(2, numSamples);
//        stereoBuffer.clear();
//        
//        stereoBuffer.copyFrom(0, 0, mono, numSamples); // Left
//        stereoBuffer.copyFrom(1, 0, mono, numSamples); // Right
//        stere
        
        buffer.clear();
        buffer.setSize(2, numSamples);
        buffer.copyFrom(0, 0, monoBuffer, 0, 0, monoBuffer.getNumSamples());
        buffer.copyFrom(1, 0, monoBuffer, 0, 0, monoBuffer.getNumSamples());
    }
    else if (numInputChannels == 2)
    {
        // Optional pseudo-mono detection (disable if you want to preserve stereo width)
        const float leftLevel  = buffer.getRMSLevel(0, 0, numSamples);
        const float rightLevel = buffer.getRMSLevel(1, 0, numSamples);

        if (leftLevel > 0.0001f && rightLevel < 0.0001f)
            buffer.copyFrom(1, 0, buffer, 0, 0, numSamples);
        else if (rightLevel > 0.0001f && leftLevel < 0.0001f)
            buffer.copyFrom(0, 0, buffer, 1, 0, numSamples);
        
    }

    // Apply panning gains
    buffer.applyGain(0, 0, numSamples, leftGain);
    buffer.applyGain(1, 0, numSamples, rightGain);
}




