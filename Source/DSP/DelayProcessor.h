/*
  ==============================================================================

    DelayProcessor.h
    Created: 14 Jul 2025 8:06:22pm
    Author:  Aaron Petrini

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>

class DelayProcessor
{
public:
    enum class Mode { Digital, Tape };

    DelayProcessor();
    void prepare(double sampleRate, int samplesPerBlock, int numChannels);
    void process(juce::AudioBuffer<float>& buffer, int numSamples, bool isMono);
    
    void setDelayTime(float timeMs);
    void setFeedback(float feedback);
    void setMix(float mix);
    void setMode(Mode newMode);
    void setSyncEnabled(bool enabled);
    void setHostBpm(float bpm);
    void setSubdivision(int subdivisionIndex);

private:
    juce::AudioBuffer<float> delayBuffer;
    int writePosition = 0;

    float delayTimeMs = 500.0f;
    float feedback = 0.5f;
    float mix = 0.5f;
    Mode mode = Mode::Digital;

    double sampleRate = 44100.0;
    bool syncEnabled = false;
    float hostBpm = 120.0f;
    int subdivisionIndex = 2; // Default to 1/4 note

    float getDelayInSamples() const;
};

