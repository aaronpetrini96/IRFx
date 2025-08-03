/*
  ==============================================================================

    Saturation.cpp
    Created: 13 Jul 2025 7:34:49pm
    Author:  Aaron Petrini

  ==============================================================================
*/

#include "Saturation.h"

Saturation::Saturation()
{
}

//void Saturation::prepare(const juce::dsp::ProcessSpec& spec)
//{
//    saturationPreEQ.state = Coefficients::makeLowShelf(spec.sampleRate, 150.f, 0.707f, juce::Decibels::decibelsToGain(2.f));
//    saturationPostEQ.state = Coefficients::makeLowPass(spec.sampleRate, 15000.f);
//    saturationPreEQ.prepare(spec);
//    saturationPostEQ.prepare(spec);
//}
void Saturation::prepare(const juce::dsp::ProcessSpec& spec)
{
    auto specCopy = spec; //because we pass a reference
    //Mono or Stereo support
    specCopy.numChannels = juce::jmin(2u, spec.numChannels);
    
    saturationPreEQ.state = Coefficients::makeLowShelf(spec.sampleRate, 150.f, 0.707f, juce::Decibels::decibelsToGain(2.f));
    saturationPostEQ.state = Coefficients::makeLowPass(spec.sampleRate, 15000.f);
    
    saturationPreEQ.prepare(specCopy);
    saturationPostEQ.prepare(specCopy);
}

void Saturation::reset()
{
    saturationPreEQ.reset();
    saturationPostEQ.reset();
}

float Saturation::processSample(float x, float drive, Type type)
{
    float mappedDrive = juce::jmap(drive, 0.0f, 12.0f, 1.0f, 10.0f);
    x *= mappedDrive;
    
    switch (type)
    {
        case Type::Neve: return neve(x) * gainFactor;
        case Type::SSL:  return ssl(x) * gainFactor;
        case Type::API:  return api(x) * gainFactor;
        default:         return x;
    }
}

void Saturation::processBlock(juce::AudioBuffer<float>& buffer, float drive, int typeIndex, float mix)
{
    const int numChannels = buffer.getNumChannels();
    juce::dsp::AudioBlock<float> block(buffer.getArrayOfWritePointers(), static_cast<size_t>(numChannels), static_cast<size_t>(buffer.getNumChannels()));
//    juce::dsp::AudioBlock<float> block(buffer);

    // Pre-EQ
    juce::dsp::ProcessContextReplacing<float> preCtx(block);
    saturationPreEQ.process(preCtx);

    juce::AudioBuffer<float> dryCopy;
    dryCopy.makeCopyOf(buffer);
    
    switch (typeIndex)
    {
        case 0: currentType = Type::Neve; break;
        case 1: currentType = Type::SSL; break;
        case 2: currentType = Type::API; break;
        default: currentType = Type::Neve; break;  // default fallback
    }

    for (int ch = 0; ch < buffer.getNumChannels(); ++ch)
    {
        auto* samples = buffer.getWritePointer(ch);

        for (int i = 0; i < buffer.getNumSamples(); ++i)
        {
            float dry = samples[i];
            float wet = processSample(dry, drive, currentType);
            samples[i] = dry * (1.0f - mix) + wet * mix;
        }
    }

    // Post-EQ
    juce::dsp::ProcessContextReplacing<float> postCtx(block);
    saturationPostEQ.process(postCtx);
}

float Saturation::neve(float x)
{
    //    Asymmetric Dist
    float asym = 0.1f * x * x * x;
    float saturated = std::tanh(x + asym);
    return saturated * 0.7f; // attenuation to avoid over-brightening
}

float Saturation::ssl(float x)
{
    // Pure symmetrical soft clip, no asymmetry
    float saturated = std::tanh(x);
    return saturated * 0.7f; // attenuation to avoid over-brightening
}

float Saturation::api(float x)
{
    // Stronger asymmetry, more "bite"
    float asym = 0.3f * x * x * x;
    float saturated = std::tanh(x + asym);
    return saturated * 0.7f; // attenuation to avoid over-brightening
}

