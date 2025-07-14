/*
  ==============================================================================

    DelayProcessor.cpp
    Created: 14 Jul 2025 8:06:22pm
    Author:  Aaron Petrini

  ==============================================================================
*/

#include "DelayProcessor.h"

DelayProcessor::DelayProcessor()
{
}

void DelayProcessor::prepare(double newSampleRate, int samplesPerBlock, int numChannels)
{
    sampleRate = newSampleRate;
    const int maxDelaySamples = static_cast<int>(sampleRate * 2.0); // up to 2 sec delay
    delayBuffer.setSize(numChannels, maxDelaySamples);
    delayBuffer.clear();
    writePosition = 0;
}

float DelayProcessor::getDelayInSamples() const
{
    if (syncEnabled)
    {
        // Assuming quarter note delay
        double beatsPerSecond = hostBpm / 60.0;
        double secondsPerBeat = 1.0 / beatsPerSecond;
        return static_cast<float>(secondsPerBeat * sampleRate);
    }
    else
    {
        return juce::jlimit(1.0f, static_cast<float>(delayBuffer.getNumSamples()) - 1.0f,
                            delayTimeMs / 1000.0f * static_cast<float>(sampleRate));
    }
}

void DelayProcessor::process(juce::AudioBuffer<float>& buffer, int numSamples, bool isMono)
{
    int numChannels = buffer.getNumChannels();
    float delaySamples = getDelayInSamples();

    for (int channel = 0; channel < numChannels; ++channel)
    {
        float* channelData = buffer.getWritePointer(channel);
        float* delayData = delayBuffer.getWritePointer(juce::jmin(channel, delayBuffer.getNumChannels() - 1));

        int localWritePos = writePosition;

        for (int i = 0; i < numSamples; ++i)
        {
            int delayReadPos = (localWritePos + delayBuffer.getNumSamples() - static_cast<int>(delaySamples)) % delayBuffer.getNumSamples();
            float delayedSample = delayData[delayReadPos];

            if (mode == Mode::Tape)
            {
                // Simple tape-style smoothing (low-pass)
                delayedSample = 0.5f * delayedSample + 0.5f * delayData[(delayReadPos + delayBuffer.getNumSamples() - 1) % delayBuffer.getNumSamples()];
            }

            float inputSample = channelData[i];
            float outputSample = inputSample * (1.0f - mix) + delayedSample * mix;

            channelData[i] = outputSample;

            // Feedback write
            delayData[localWritePos] = inputSample + delayedSample * feedback;

            localWritePos = (localWritePos + 1) % delayBuffer.getNumSamples();
        }
    }

    // If mono, copy first channel to others
    if (isMono && numChannels > 1)
    {
        for (int ch = 1; ch < numChannels; ++ch)
        {
            buffer.copyFrom(ch, 0, buffer, 0, 0, numSamples);
        }
    }

    writePosition = (writePosition + numSamples) % delayBuffer.getNumSamples();
}

void DelayProcessor::setDelayTime(float timeMs)
{
    delayTimeMs = timeMs;
}
void DelayProcessor::setFeedback(float fb)
{
    fb *= 0.01f;
    feedback = juce::jlimit(0.0f, 0.99f, fb);
}
void DelayProcessor::setMix(float m)
{
    m *= 0.01f;
    mix = juce::jlimit(0.0f, 1.0f, m);
}
void DelayProcessor::setMode(Mode newMode)
{
    mode = newMode;
}
void DelayProcessor::setSyncEnabled(bool enabled)
{
    syncEnabled = enabled;
}
void DelayProcessor::setHostBpm(float bpm)
{
    hostBpm = bpm;
}

