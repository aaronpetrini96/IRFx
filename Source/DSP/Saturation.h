/*
  ==============================================================================

    Saturation.h
    Created: 13 Jul 2025 7:34:49pm
    Author:  Aaron Petrini

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>

class Saturation
{
public:
    Saturation();
    
    enum class Type
    {
        Neve = 0,
        SSL = 1,
        API = 2
    };

    void prepare(const juce::dsp::ProcessSpec& spec);
    void reset();
    float processSample(float x, float drive, Type type);
    void processBlock(juce::AudioBuffer<float>& buffer, float drive, int typeIndex, float mix);
    void resizeAndPrepareFiltersIfNeeded(int numChannels, double sr);

private:
    using Coefficients = juce::dsp::IIR::Coefficients<float>;
    using Filter = juce::dsp::IIR::Filter<float>;
    juce::dsp::ProcessorDuplicator<Filter, Coefficients> saturationPreEQ, saturationPostEQ;
    std::vector<juce::dsp::ProcessorDuplicator<Filter, Coefficients>> preFilters, postFilters;
    Type currentType { Type::Neve };

    float neve(float x);
    float ssl(float x);
    float api(float x);
    float gainReductionDb = -6.0f;
    float gainFactor = std::pow(10.f, (gainReductionDb)/20.f); //Reduce output to match previous level
    double sampleRate = 44100.0;
};
