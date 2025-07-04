/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "ParamNames.h"
#include "DSP/SingleChannelSampleFifo.h"

//==============================================================================
/**
*/
class IRFxAudioProcessor  : public juce::AudioProcessor
{
public:
    //==============================================================================
    IRFxAudioProcessor();
    ~IRFxAudioProcessor() override;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;
       
    //======================DSP FUNCTIONS==========================================
    
    void loadIR1(const juce::File&);
    void loadIR2(const juce::File&);
    bool isIR1Loaded {false}, isIR2Loaded {false};
    bool isIR1Muted {false}, isIR2Muted {false};
    
    void updateParams();
    
    //==============================================================================
    juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();
    juce::AudioProcessorValueTreeState apvts {*this, nullptr, "Parameters", createParameterLayout()};
    
    //======================= PARAMETERS PTRS ================================================
    
    //IR MODULE
    juce::AudioParameterFloat* lowCutFreqParam {nullptr};
    juce::AudioParameterFloat* highCutFreqParam {nullptr};
    juce::AudioParameterBool* irLoaderBypassParam {nullptr};
    
    //TONE STACK
    juce::AudioParameterFloat* lowEQGainParam {nullptr};
    juce::AudioParameterFloat* midEQGainParam {nullptr};
    juce::AudioParameterFloat* midEQFreqParam {nullptr};
    juce::AudioParameterFloat* highEQGainParam {nullptr};
    juce::AudioParameterBool* eqBypassParam {nullptr};
    
    //SATURATION
    
    //DELAY
    
    //======================= SMOOTHED PARAMS ================================================
    juce::SmoothedValue<float>
    lowCutFreqParamSmoother,
    highCutFreqParamSmoother,
    lowEQGainParamSmoother,
    midEQGainParamSmoother,
    midEQFreqParamSmoother,
    highEQGainParamSmoother;
    
    juce::dsp::ProcessSpec spec;
    juce::dsp::Convolution irLoader1, irLoader2;

private:
    
    
    float mixIR1, mixIR2;
    
    using Filter = juce::dsp::IIR::Filter<float>;
    using irEQMonoChain = juce::dsp::ProcessorChain<Filter, Filter>;
    std::array<irEQMonoChain, 2> irEQMonoChainArray;
//    juce::LinearSmoothedValue<float> smoothedIRBypassValue {1.f},
//                                     smoothedEQBypassValue {1.f},
//                                     smoothedDistBypassValue {1.f},
//                                     smoothedDelayBypassValue {1.f};
    
    using toneStackMonoChain = juce::dsp::ProcessorChain<Filter, Filter, Filter>;
    std::array<toneStackMonoChain, 3> toneStackMonoChainAray;
    
    float lowShelfGain, midPeakGain, midPeakFreq, highShelfGain;
    
    
    template<typename ParamType, typename Params, typename Funcs>
    void initCachedParams(Params paramsArray, Funcs funcsArray)
    {
        for (size_t i = 0 ; i < paramsArray.size(); ++i)
        {
            auto ptrToParamPtr = paramsArray [i];
            *ptrToParamPtr = dynamic_cast<ParamType>(apvts.getParameter(funcsArray[i]()));
            jassert(*ptrToParamPtr != nullptr);
        }
    }
    
    
    std::vector<juce::SmoothedValue<float>*> getSmoothers();
    
    enum class SmootherUpdateMode
    {
        initialize,
        liveInRealTime
    };
    
    void updateSmootherFromParams(int numSamplesToSkip, SmootherUpdateMode init);
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (IRFxAudioProcessor)
};
