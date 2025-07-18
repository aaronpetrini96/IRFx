/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "ParamNames.h"
#include "DSP/Saturation.h"
#include "DSP/DelayProcessor.h"

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
//    void checkAndLoadPendingIRs();
    bool isIR1Loaded {false}, isIR2Loaded {false};
    bool isIR1Muted {false}, isIR2Muted {false};
    
    void updateParams();
    
    float getInputLevelL() const { return inputLevelL; }
    float getInputLevelR() const { return inputLevelR; }
    float getOutputLevelL() const { return outputLevelL; }
    float getOutputLevelR() const { return outputLevelR; }
    
    
    void savePreset(const juce::File& file);
    void loadPreset(const juce::File& file);
    void loadDefaultPreset();
    
    //==============================================================================
    juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();
    juce::AudioProcessorValueTreeState apvts {*this, nullptr, "Parameters", createParameterLayout()};
    
    //======================= PARAMETERS PTRS ================================================
    
    //IR MODULE
    juce::AudioParameterFloat* lowCutFreqParam {nullptr};
    juce::AudioParameterFloat* highCutFreqParam {nullptr};
    juce::AudioParameterFloat* ir1LevelParam {nullptr};
    juce::AudioParameterFloat* ir2LevelParam {nullptr};
    juce::AudioParameterBool* irLoaderBypassParam {nullptr};
    
    //TONE STACK
    juce::AudioParameterFloat* lowEQGainParam {nullptr};
    juce::AudioParameterFloat* midEQGainParam {nullptr};
    juce::AudioParameterFloat* midEQFreqParam {nullptr};
    juce::AudioParameterFloat* highEQGainParam {nullptr};
    juce::AudioParameterBool* eqBypassParam {nullptr};
    
    //SATURATION
    juce::AudioParameterFloat* saturationDriveParam {nullptr};
    juce::AudioParameterFloat* saturationMixParam {nullptr};
    juce::AudioParameterBool* saturationBypassParam {nullptr};
    juce::AudioParameterChoice* saturationModeParam {nullptr};
    
    //DELAY
    juce::AudioParameterFloat* delayMixParam {nullptr};
    juce::AudioParameterFloat* delayFeedbackParam {nullptr};
    juce::AudioParameterFloat* delayTimeParam {nullptr};
    juce::AudioParameterChoice* delayModeParam {nullptr};
    juce::AudioParameterChoice* delayMonoStereoParam {nullptr};
    juce::AudioParameterBool* delaySyncParam {nullptr};
    juce::AudioParameterChoice* delayNoteParam {nullptr};
    juce::AudioParameterBool* delayBypassParam{nullptr};

    
    //IN-OUT GAIN
    juce::AudioParameterFloat* inputGainParam {nullptr};
    juce::AudioParameterFloat* outputGainParam {nullptr};
    
    juce::AudioParameterBool* pluginBypassParam {nullptr};
    
    //======================= SMOOTHED PARAMS ================================================
    juce::SmoothedValue<float>
    lowCutFreqParamSmoother,
    highCutFreqParamSmoother,
    ir1LevelParamSmoother,
    ir2LevelParamSmoother,
    lowEQGainParamSmoother,
    midEQGainParamSmoother,
    midEQFreqParamSmoother,
    highEQGainParamSmoother,
    saturationDriveParamSmoother,
    saturationMixParamSmoother,
    delayMixParamSmoother,
    delayFeedbackParamSmoother,
    delayTimeParamSmoother,
    inputGainParamSmoother,
    outputGainParamSmoother;
    
    juce::dsp::ProcessSpec spec;
    std::unique_ptr<juce::dsp::Convolution> irLoader1, irLoader2;
    
    std::atomic<bool> clipFlagIn { false };
    std::atomic<bool> clipFlagOut { false };
    

    
    std::atomic<bool> ir1PendingUpdate { false };
    juce::File irFile1ToLoad;
    std::unique_ptr<juce::dsp::Convolution> pendingIR1;
    std::atomic<bool> ir2PendingUpdate { false };
    juce::File irFile2ToLoad;
    std::unique_ptr<juce::dsp::Convolution> pendingIR2;



private:
    
    float inputLevelL{0.f}, inputLevelR {0.f}, outputLevelL{0.f}, outputLevelR{0.f};
    juce::dsp::Gain<float> inputGain, outputGain;
    juce::dsp::Gain<float> gain;
    float mixIR1, mixIR2;
    template<typename T, typename U>
    void applyGain(T& buffer, U& gain)
    {
        auto block = juce::dsp::AudioBlock<float>(buffer);
        auto ctx = juce::dsp::ProcessContextReplacing<float>(block);
        gain.process(ctx);
    }
    
    
    
    using Filter = juce::dsp::IIR::Filter<float>;
    using Coefficients = juce::dsp::IIR::Coefficients<float>;
    using irEQMonoChain = juce::dsp::ProcessorChain<Filter, Filter>;
    std::array<irEQMonoChain, 2> irEQMonoChainArray;
//    juce::LinearSmoothedValue<float> smoothedIRBypassValue {1.f},
//                                     smoothedEQBypassValue {1.f},
//                                     smoothedDistBypassValue {1.f},
//                                     smoothedDelayBypassValue {1.f};
    
    using toneStackMonoChain = juce::dsp::ProcessorChain<Filter, Filter, Filter>;
    std::array<toneStackMonoChain, 3> toneStackMonoChainAray;
    float lowShelfGain, midPeakGain, midPeakFreq, highShelfGain;
    
    
    Saturation saturationInstance;


    DelayProcessor delayInstance;
    bool isMono = true;
    
    
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
