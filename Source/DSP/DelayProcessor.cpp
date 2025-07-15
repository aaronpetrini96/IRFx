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
//    delayBuffer.setSize(2, maxDelaySamples); // Stereo buffer for ping-pong
    delayBuffer.clear();
    writePosition = 0;
}

float DelayProcessor::getDelayInSamples() const
{
    if (syncEnabled)
    {
        // Calculate beat duration
        double beatTimeSec = 60.0 / static_cast<double>(hostBpm);
        double delayTimeSec = beatTimeSec;

        switch (subdivisionIndex)
        {
            case 0: delayTimeSec = beatTimeSec * 4.0; break;          // 1/1
            case 1: delayTimeSec = beatTimeSec * 2.0; break;          // 1/2
            case 2: delayTimeSec = beatTimeSec * 1.0; break;          // 1/4
            case 3: delayTimeSec = beatTimeSec * 0.5; break;          // 1/8
            case 4: delayTimeSec = beatTimeSec * 0.25; break;         // 1/16
            case 5: delayTimeSec = beatTimeSec * 1.5; break;          // dotted 1/4
            case 6: delayTimeSec = beatTimeSec * (1.0 / 3.0); break;  // triplet 1/4
            default: delayTimeSec = beatTimeSec * 1.0; break;         // default to 1/4
        }

        float delaySamples = static_cast<float>(delayTimeSec * sampleRate);
        return juce::jlimit(1.0f, static_cast<float>(delayBuffer.getNumSamples()) - 1.0f, delaySamples);
    }
    else
    {
        float delaySamples = delayTimeMs / 1000.0f * static_cast<float>(sampleRate);
        return juce::jlimit(1.0f, static_cast<float>(delayBuffer.getNumSamples()) - 1.0f, delaySamples);
    }
}

void DelayProcessor::process(juce::AudioBuffer<float>& buffer, int numSamples, bool isMono)
{
    float delaySamples = getDelayInSamples();
    int bufferSize = delayBuffer.getNumSamples();
    int numInputChannels = buffer.getNumChannels();

    // Ensure buffer has 2 channels for ping-pong output
    buffer.setSize(2, numSamples, true, true, true);

    auto* leftChannelData = buffer.getWritePointer(0);
    auto* rightChannelData = buffer.getWritePointer(1);

    auto* leftDelayData = delayBuffer.getWritePointer(0);
    auto* rightDelayData = delayBuffer.getWritePointer(1);

    // Static filter states for tape low-pass (consider member variables for thread safety)
    static float prevFilteredL = 0.0f;
    static float prevFilteredR = 0.0f;
    const float filterCoeff = 0.2f;  // Adjust for tone darkness

    int localWritePos = writePosition;

    for (int i = 0; i < numSamples; ++i)
    {
        int delayReadPos = (localWritePos + bufferSize - static_cast<int>(delaySamples)) % bufferSize;

        float delayedL = leftDelayData[delayReadPos];
        float delayedR = rightDelayData[delayReadPos];

        if (this->mode == Mode::Tape)
        {
            // Stronger low-pass filtering on delayed signals
            prevFilteredL = (1.0f - filterCoeff) * prevFilteredL + filterCoeff * delayedL;
            prevFilteredR = (1.0f - filterCoeff) * prevFilteredR + filterCoeff * delayedR;

            delayedL = prevFilteredL;
            delayedR = prevFilteredR;

            // Saturate delayed signals for analog tape warmth
            delayedL = std::tanh(delayedL * 1.5f);
            delayedR = std::tanh(delayedR * 1.5f);
        }

        float inputSampleL = numInputChannels > 0 ? buffer.getReadPointer(0)[i] : 0.0f;
        float inputSampleR = numInputChannels > 1 ? buffer.getReadPointer(1)[i] : inputSampleL;

        float dryL = inputSampleL * (1.0f - mix);
        float dryR = inputSampleR * (1.0f - mix);

        float wetL = 0.0f;
        float wetR = 0.0f;

        if (isMono)
        {
            // Mono delay: average inputs, write same feedback to both delay buffers
            float monoInput = 0.5f * (inputSampleL + inputSampleR);
            float monoDelayed = delayedL; // Use left delay buffer for mono output

            wetL = monoDelayed * mix;
            wetR = monoDelayed * mix;

            // Saturate feedback if tape mode
            float feedbackSample = monoInput + monoDelayed * feedback;
            if (this->mode == Mode::Tape)
                feedbackSample = std::tanh(feedbackSample * 1.5f);

            leftDelayData[localWritePos] = feedbackSample;
            rightDelayData[localWritePos] = feedbackSample;
        }
        else
        {
            // Ping-pong delay: cross feedback
            wetL = delayedL * mix;
            wetR = delayedR * mix;

            // Saturate feedback if tape mode
            float feedbackSampleL = 0.5f * (inputSampleL + inputSampleR) + delayedR * feedback;
            float feedbackSampleR = 0.5f * (inputSampleL + inputSampleR) + delayedL * feedback;

            if (this->mode == Mode::Tape)
            {
                feedbackSampleL = std::tanh(feedbackSampleL * 1.5f);
                feedbackSampleR = std::tanh(feedbackSampleR * 1.5f);
            }

            leftDelayData[localWritePos] = feedbackSampleL;
            rightDelayData[localWritePos] = feedbackSampleR;
        }

        leftChannelData[i] = dryL + wetL;
        rightChannelData[i] = dryR + wetR;

        localWritePos = (localWritePos + 1) % bufferSize;
    }

    writePosition = (writePosition + numSamples) % bufferSize;
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
void DelayProcessor::setSubdivision(int idx)
{
    subdivisionIndex = idx;
}

