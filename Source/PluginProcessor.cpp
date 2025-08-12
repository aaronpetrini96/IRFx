/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
IRFxAudioProcessor::IRFxAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       )
#endif
{

//============ FLOAT PARAMS ============
    auto floatParams = std::array
    {
//        IR LOADER
        &lowCutFreqParam,
        &highCutFreqParam,
        &ir1LevelParam,
        &ir2LevelParam,
        &ir1PanParam,
        &ir2PanParam,
        
//        EQ
        &lowEQGainParam,
        &midEQGainParam,
        &midEQFreqParam,
        &highEQGainParam,
        
//        SAT
        &saturationDriveParam,
        &saturationMixParam,
        
//        DELAY
        &delayMixParam,
        &delayFeedbackParam,
        &delayTimeParam,

        
//        IN-OUT GAIN
        &inputGainParam,
        &outputGainParam,
    };
    
    auto floatNameFuncs = std::array
    {
        //        IR LOADER
        &ParamNames::getIRLowCutName,
        &ParamNames::getIRHighCutName,
        &ParamNames::getIR1LevelName,
        &ParamNames::getIR2LevelName,
        &ParamNames::getIR1PanName,
        &ParamNames::getIR2PanName,
        
        //        EQ
        &ParamNames::getEQLowGainName,
        &ParamNames::getEQMidGainName,
        &ParamNames::getEQMidFreqName,
        &ParamNames::getEQHighGainName,
        
        //        SAT
        &ParamNames::getDistDriveName,
        &ParamNames::getDistMixName,
        
        //        DELAY
        &ParamNames::getDelayMixName,
        &ParamNames::getDelayFeedbackName,
        &ParamNames::getDelayTimeName,
        
        //        IN-OUT GAIN
        &ParamNames::getInGainName,
        &ParamNames::getOutGainName,
    };
    
    initCachedParams<juce::AudioParameterFloat*>(floatParams, floatNameFuncs);
    
    
    //============ BOOL PARAMS ============
    auto boolParams = std::array
    {
        //        IR LOADER
        &irLoaderBypassParam,
        //        EQ
        &eqBypassParam,
        //        SAT
        &saturationBypassParam,
        //        DELAY
        &delayBypassParam,
        &delaySyncParam,
        
        &pluginBypassParam,
    };
    
    auto boolNameFuncs = std::array
    {
        //        IR LOADER
        &ParamNames::getIRBypassName,
        //        EQ
        &ParamNames::getEQBypassName,
        
        //        SAT
        &ParamNames::getDistBypassName,
        //        DELAY
        &ParamNames::getDelayBypassName,
        &ParamNames::getDelaySyncName,
        
        &ParamNames::getGeneralBypassName,
    };
    
    initCachedParams<juce::AudioParameterBool*>(boolParams, boolNameFuncs);
    
    //============ CHOICE PARAMS ============
    auto choiceParams = std::array
    {
//        DIST
        &saturationModeParam,
//        DELAY
        &delayModeParam,
        &delayNoteParam,
        
        &outputMonoStereoParam,
    };
    
    auto choiceNameFuncs = std::array
    {
        &ParamNames::getDistModeName,
        &ParamNames::getDelayModeName,
        &ParamNames::getDelayNoteName,
        &ParamNames::getOutputMonoStereoName,
    };
    
    initCachedParams<juce::AudioParameterChoice*>(choiceParams, choiceNameFuncs);
    
}

IRFxAudioProcessor::~IRFxAudioProcessor()
{
}

//==============================================================================
const juce::String IRFxAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool IRFxAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool IRFxAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool IRFxAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double IRFxAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int IRFxAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int IRFxAudioProcessor::getCurrentProgram()
{
    return 0;
}

void IRFxAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String IRFxAudioProcessor::getProgramName (int index)
{
    return {};
}

void IRFxAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}
//==============================================================================

juce::AudioProcessorValueTreeState::ParameterLayout IRFxAudioProcessor::createParameterLayout()
{
    std::vector <std::unique_ptr<juce::RangedAudioParameter>> params;
    const int versionHint = 1;
    
//    IR Loader
    auto name = ParamNames::getIRBypassName();
    params.emplace_back(std::make_unique<juce::AudioParameterBool>(juce::ParameterID(name, versionHint), name, false));
    name = ParamNames::getIRLowCutName();
    params.emplace_back(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID(name, versionHint), name, juce::NormalisableRange<float>(20.f, 1000.f, 1.f, 0.8f), 20.f));
    name = ParamNames::getIRHighCutName();
    params.emplace_back(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID(name, versionHint), name, juce::NormalisableRange<float>(1000.f, 20000.f, 1.f, 1.f), 20000.f));
    name = ParamNames::getIR1LevelName();
    params.emplace_back(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID(name, versionHint), name, juce::NormalisableRange<float>(-60.f, 0.f, 0.1f, 1.f), 0.f));
    name = ParamNames::getIR2LevelName();
    params.emplace_back(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID(name, versionHint), name, juce::NormalisableRange<float>(-60.f, 0.f, 0.1f, 1.f), 0.f));
    name = ParamNames::getIR1PanName();
    params.emplace_back(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID(name, versionHint), name, juce::NormalisableRange<float>(-100.f, 100.f, 1.f, 1.f), 0.f));
    name = ParamNames::getIR2PanName();
    params.emplace_back(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID(name, versionHint), name, juce::NormalisableRange<float>(-100.f, 100.f, 1.f, 1.f), 0.f));
    
//   EQ
    name = ParamNames::getEQBypassName();
    params.emplace_back(std::make_unique<juce::AudioParameterBool>(juce::ParameterID(name, versionHint), name, false));
    name = ParamNames::getEQLowGainName();
    params.emplace_back(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID(name, versionHint), name, juce::NormalisableRange<float>(-12.f, 12.f, 0.1f, 1.f), 0.f));
    name = ParamNames::getEQMidGainName();
    params.emplace_back(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID(name, versionHint), name, juce::NormalisableRange<float>(-12.f, 12.f, 0.1f, 1.f), 0.f));
    name = ParamNames::getEQMidFreqName();
    params.emplace_back(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID(name, versionHint), name, juce::NormalisableRange<float>(250.f, 5000.f, 1.f, 1.f), 550.f));
    name = ParamNames::getEQHighGainName();
    params.emplace_back(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID(name, versionHint), name, juce::NormalisableRange<float>(-12.f, 12.f, 0.1f, 1.f), 0.f));
    
//   DIST
    name = ParamNames::getDistBypassName();
    params.emplace_back(std::make_unique<juce::AudioParameterBool>(juce::ParameterID(name, versionHint), name, false));
    name = ParamNames::getDistDriveName();
    params.emplace_back(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID(name, versionHint), name, juce::NormalisableRange<float>(0.f, 12.f, 0.1f, 1.f), 6.f));
    name = ParamNames::getDistMixName();
    params.emplace_back(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID(name, versionHint), name, juce::NormalisableRange<float>(0.f, 100.f, 1.f, 1.f), 0.f));
    name = ParamNames::getDistModeName();
    juce::StringArray choices;
    choices.add("Neve");
    choices.add("SSL");
    choices.add("API");
    params.emplace_back(std::make_unique<juce::AudioParameterChoice>(juce::ParameterID(name, versionHint), name, choices, 0));
    
//   DELAY
    name = ParamNames::getDelayBypassName();
    params.emplace_back(std::make_unique<juce::AudioParameterBool>(juce::ParameterID(name, versionHint), name, false));
    name = ParamNames::getDelayMixName();
    params.emplace_back(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID(name, versionHint), name, juce::NormalisableRange<float>(0.f, 100.f, 1.f, 1.f), 0.f));
    name = ParamNames::getDelayFeedbackName();
    params.emplace_back(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID(name, versionHint), name, juce::NormalisableRange<float>(0.f, 100.f, 1.f, 1.f), 30.f));
    name = ParamNames::getDelayTimeName();
    params.emplace_back(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID(name, versionHint), name, juce::NormalisableRange<float>(1.f, 2000.f, 1.f, 0.5f), 375.f));
    name = ParamNames::getDelayModeName();
    juce::StringArray delayModes;
    delayModes.add("Digital");
    delayModes.add("Tape");
    params.emplace_back(std::make_unique<juce::AudioParameterChoice>(juce::ParameterID(name, versionHint), name, delayModes, 0));
    name = ParamNames::getDelaySyncName();
    params.emplace_back(std::make_unique<juce::AudioParameterBool>(juce::ParameterID(name, versionHint), name, false));
    name = ParamNames::getDelayNoteName();
    juce::StringArray subdivisionArray { "1/1", "1/2", "1/4", "1/8", "1/16", "1/4 Dotted", "1/4 Triplet", "1/32"};
    params.emplace_back(std::make_unique<juce::AudioParameterChoice>(juce::ParameterID(name, versionHint), name, subdivisionArray, 2));

    
//   IN-OUT GAIN
    name = ParamNames::getInGainName();
    params.emplace_back(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID(name, versionHint), name, juce::NormalisableRange<float>(-60.f, 12.f, 0.1f, 1.f), 0.f));
    name = ParamNames::getOutGainName();
    params.emplace_back(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID(name, versionHint), name, juce::NormalisableRange<float>(-60.f, 12.f, 0.1f, 1.f), 0.f));
    
    
//   PLUGIN BYPASS
    name = ParamNames::getGeneralBypassName();
    params.emplace_back(std::make_unique<juce::AudioParameterBool>(juce::ParameterID(name, versionHint), name, false));
    
//    PLUGIN OUTPUT
    name = ParamNames::getOutputMonoStereoName();
    juce::StringArray outputMonoStereoArray {"Mono", "Stereo"};
    params.emplace_back(std::make_unique<juce::AudioParameterChoice>(juce::ParameterID(name, versionHint), name, outputMonoStereoArray, 1));
    

    return {params.begin(), params.end()};
}

void IRFxAudioProcessor::updateParams()
{
    auto sampleRate = getSampleRate();
    using Coefficients = juce::dsp::IIR::Coefficients<float>;
    
//    IR LOADER
    auto lowCutCoefficients = Coefficients::makeHighPass(sampleRate, lowCutFreqParamSmoother.getCurrentValue());
    auto highCutCoefficients = Coefficients::makeLowPass(sampleRate, highCutFreqParamSmoother.getCurrentValue());

    for (auto& chain : irEQMonoChainArray)
    {
        auto& lowCut = chain.template get<0>();
        auto& highCut = chain.template get<1>();
        
        *lowCut.coefficients = *lowCutCoefficients;
        *highCut.coefficients = *highCutCoefficients;
    }
    
//    EQ STACK
    lowShelfGain = juce::Decibels::decibelsToGain(lowEQGainParamSmoother.getCurrentValue());
    midPeakGain = juce::Decibels::decibelsToGain(midEQGainParamSmoother.getCurrentValue());
    midPeakFreq = midEQFreqParamSmoother.getCurrentValue();
    highShelfGain = juce::Decibels::decibelsToGain(highEQGainParamSmoother.getCurrentValue());
    
    auto lowEQCoefficients = Coefficients::makeLowShelf(sampleRate, 110.f, 0.707f, lowShelfGain);
    
    auto midEQCoefficients = Coefficients::makePeakFilter(sampleRate, midPeakFreq, 1.f, midPeakGain);
    auto highEQCoefficients = Coefficients::makeHighShelf(sampleRate, 4500.f, 0.707f, highShelfGain);
    
    for (auto& chain : toneStackMonoChainAray)
    {
        auto& lowShelf = chain.template get<0>();
        auto& midPeak = chain.template get<1>();
        auto& highShelf = chain.template get<2>();
        
        *lowShelf.coefficients = *lowEQCoefficients;
        *midPeak.coefficients = *midEQCoefficients;
        *highShelf.coefficients = *highEQCoefficients;
    }
}
//==============================================================================
//==============================================================================
void IRFxAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    spec.maximumBlockSize = samplesPerBlock;
    spec.sampleRate = sampleRate;
    spec.numChannels = getTotalNumOutputChannels();
    
    outputIsStereo = outputMonoStereoParam->getIndex() == 1;
    
    if (deferredIR1File.existsAsFile())
        loadIR1(deferredIR1File);

    if (deferredIR2File.existsAsFile())
        loadIR2(deferredIR2File);
    
    irLoader1 = std::make_unique<juce::dsp::Convolution>();
    irLoader2 = std::make_unique<juce::dsp::Convolution>();
    irLoader1->reset();
    irLoader2->reset();
    irLoader1->prepare(spec);
    irLoader2->prepare(spec);
    
    spec.numChannels = getTotalNumOutputChannels();
    inputGain.prepare(spec);
    inputGain.setRampDurationSeconds(0.05);
    outputGain.prepare(spec);
    outputGain.setRampDurationSeconds(0.05);
    
    gain.prepare(spec);
    gain.setGainDecibels(-12.f);
    
    juce::dsp::ProcessSpec monoSpec = spec;
    monoSpec.numChannels = 1;  // IMPORTANT: mono
    
    for (int i = 0; i < getTotalNumOutputChannels(); ++i)
    {
        irEQMonoChainArray[i].reset();
        irEQMonoChainArray[i].prepare(monoSpec);
        toneStackMonoChainAray[i].reset();
        toneStackMonoChainAray[i].prepare(monoSpec);
    }
 
    saturationInstance.prepare(spec);
    
    delayInstance.prepare(sampleRate, samplesPerBlock, getTotalNumOutputChannels());
    
    for(auto smoother : getSmoothers())
        smoother->reset(sampleRate, 0.05);
    
    updateSmootherFromParams(1, SmootherUpdateMode::initialize);
    updateParams();
}

void IRFxAudioProcessor::updateSmootherFromParams(int numSamplesToSkip, SmootherUpdateMode init)
{
    auto paramsNeedingSmoothing = std::vector
    {
        lowCutFreqParam,
        highCutFreqParam,
        ir1LevelParam,
        ir2LevelParam,
        ir1PanParam,
        ir2PanParam,
        lowEQGainParam,
        midEQGainParam,
        midEQFreqParam,
        highEQGainParam,
        saturationDriveParam,
        saturationMixParam,
        delayMixParam,
        delayFeedbackParam,
        delayTimeParam,
        inputGainParam,
        outputGainParam,
    };
    
    auto smoothers = getSmoothers();
    jassert(smoothers.size() == paramsNeedingSmoothing.size());
    
    for (size_t i = 0; i < smoothers.size(); ++i)
    {
        auto smoother = smoothers[i];
        auto param = paramsNeedingSmoothing [i];
        
        if (init == SmootherUpdateMode::initialize)
            smoother->setCurrentAndTargetValue(param->get());
        else
            smoother->setTargetValue(param->get());
        
        smoother->skip(numSamplesToSkip);
    }
}

std::vector<juce::SmoothedValue<float>*> IRFxAudioProcessor::getSmoothers()
{
    auto smoothers = std::vector
    {
        &lowCutFreqParamSmoother,
        &highCutFreqParamSmoother,
        &ir1LevelParamSmoother,
        &ir2LevelParamSmoother,
        &ir1PanParamSmoother,
        &ir2PanParamSmoother,
        &lowEQGainParamSmoother,
        &midEQGainParamSmoother,
        &midEQFreqParamSmoother,
        &highEQGainParamSmoother,
        &saturationDriveParamSmoother,
        &saturationMixParamSmoother,
        &delayMixParamSmoother,
        &delayFeedbackParamSmoother,
        &delayTimeParamSmoother,
        &inputGainParamSmoother,
        &outputGainParamSmoother,
    };
    
    return smoothers;
}

void IRFxAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

bool IRFxAudioProcessor::isBusesLayoutSupported(const BusesLayout &layouts) const
{
    const auto mono = juce::AudioChannelSet::mono();
    const auto stereo = juce::AudioChannelSet::stereo();
    const auto mainIn = layouts.getMainInputChannelSet();
    const auto mainOut = layouts.getMainOutputChannelSet();
    
//    if (mainIn == mono && mainOut == mono) {return true;}
    if (mainIn == mono && mainOut == stereo) {return true;}
    if (mainIn == stereo && mainOut == stereo) {return true;}
    
    return false;
}


void IRFxAudioProcessor::loadIR1(const juce::File &irFile)
{
    // Store the file path for later use
    apvts.state.setProperty("IR1FilePath", irFile.getFullPathName(), nullptr);
    
    if (!irFile.existsAsFile())
        return;

    if (spec.sampleRate == 0)
    {
        deferredIR1File = irFile;
        return;
    }

    
    auto newIR = std::make_unique<juce::dsp::Convolution>();
    newIR->loadImpulseResponse(irFile,
                               juce::dsp::Convolution::Stereo::yes,
                               juce::dsp::Convolution::Trim::yes,
                               0,
                               juce::dsp::Convolution::Normalise::yes);
    
    // Prepare it here with the cached spec
    newIR->prepare(spec);
    
    pendingIR1 = std::move(newIR);
    ir1PendingUpdate.store(true);
    isIR1Loaded = true;
}


void IRFxAudioProcessor::loadIR2(const juce::File& irFile)
{
    // Store the file path for later use
    apvts.state.setProperty("IR2FilePath", irFile.getFullPathName(), nullptr);
    
    if (!irFile.existsAsFile())
        return;

    if (spec.sampleRate == 0)
    {
        deferredIR2File = irFile;
        return;
    }
    
    auto newIR = std::make_unique<juce::dsp::Convolution>();
    newIR->loadImpulseResponse(irFile,
                               juce::dsp::Convolution::Stereo::yes,
                               juce::dsp::Convolution::Trim::yes,
                               0,
                               juce::dsp::Convolution::Normalise::yes);

    newIR->prepare(spec);

    pendingIR2 = std::move(newIR);
    ir2PendingUpdate.store(true);
    isIR2Loaded = true;
}



float computeRMS(const float* data, size_t numSamples)
{
    float sumSquares = 0.0f;
    for (size_t i = 0; i < numSamples; ++i)
        sumSquares += data[i] * data[i];

    return std::sqrt(sumSquares / (float)numSamples);
}


bool clipDetection(juce::AudioBuffer<float>& buffer)
{
    bool isClipping = false;
    for (int channel = 0; channel < buffer.getNumChannels(); ++channel)
    {
        auto* samples = buffer.getReadPointer(channel);
        for (int i = 0; i < buffer.getNumSamples(); ++i)
        {
            auto sampleAbs = std::abs(samples[i]);
            if (sampleAbs > 1.0f)
            {
                isClipping = true;
                break; // you can also break out of both loops early if you want
            }
        }
    }
    
    return isClipping;
}

void IRFxAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    [[maybe_unused]] int totalNumInputChannels  = getTotalNumInputChannels();
    [[maybe_unused]] int totalNumOutputChannels = getTotalNumOutputChannels();


    if (totalNumInputChannels == 1 && totalNumOutputChannels == 2)
    {
        const int numSamples = buffer.getNumSamples();
        buffer.setSize(2, numSamples, true, true, true); // expand to stereo, keep content

        // copy left to right channel
        buffer.copyFrom(1, 0, buffer.getReadPointer(0), numSamples);
    }



    //========================                    ========================
        
    updateSmootherFromParams(buffer.getNumSamples(), SmootherUpdateMode::liveInRealTime);
    updateParams();
    
    if (pluginBypassParam->get() == false)
    {
        outputIsStereo = outputMonoStereoParam->getIndex() == 1;
        //========================    IN GAIN part    ========================
        inputGain.setGainDecibels(inputGainParamSmoother.getCurrentValue());
        applyGain(buffer, inputGain);
        
        //========================    IR LOADER part    ========================
        if (irLoaderBypassParam->get() == false)
        {
            
            // Evaluate effective loading states after mute
            const bool useIR1 = isIR1Loaded && !isIR1Muted;
            const bool useIR2 = isIR2Loaded && !isIR2Muted;

            if (useIR1 && useIR2)
            {
                if (ir1PendingUpdate.exchange(false))
                    irLoader1 = std::move(pendingIR1);
                if (ir2PendingUpdate.exchange(false))
                    irLoader2 = std::move(pendingIR2);
                
                juce::AudioBuffer<float> tempBuffer;
                tempBuffer.setSize(buffer.getNumChannels(), buffer.getNumSamples());
                tempBuffer.makeCopyOf(buffer, true);
                
                buffer.applyGain(juce::Decibels::decibelsToGain(ir1LevelParamSmoother.getCurrentValue()));
                tempBuffer.applyGain(juce::Decibels::decibelsToGain(ir2LevelParamSmoother.getCurrentValue()));
                juce::dsp::AudioBlock<float> block1(buffer);
                juce::dsp::AudioBlock<float> block2(tempBuffer);
                irLoader1->process(juce::dsp::ProcessContextReplacing<float>(block1));
                irLoader2->process(juce::dsp::ProcessContextReplacing<float>(block2));
                
                if (outputIsStereo)
                {
                    applyEqualPowerPan(buffer, ir1PanParamSmoother.getCurrentValue() * 0.01f);
                    applyEqualPowerPan(tempBuffer, ir2PanParamSmoother.getCurrentValue() * 0.01f);
                }
                else
                {
                    applyEqualPowerPan(buffer, 0.f);
                    applyEqualPowerPan(tempBuffer, 0.f);
                }

                for (int ch = 0; ch < buffer.getNumChannels(); ++ch)
                    buffer.addFrom(ch, 0, tempBuffer, ch, 0, buffer.getNumSamples());

                buffer.applyGain(juce::Decibels::decibelsToGain(3.f));
            }
            else if (useIR1)
            {
                if (ir1PendingUpdate.exchange(false))
                    irLoader1 = std::move(pendingIR1);
                buffer.applyGain(juce::Decibels::decibelsToGain(ir1LevelParamSmoother.getCurrentValue()));
                juce::dsp::AudioBlock<float> block(buffer);
                irLoader1->process(juce::dsp::ProcessContextReplacing<float>(block));
                
                if (outputIsStereo)
                {
                    applyEqualPowerPan(buffer, ir1PanParamSmoother.getCurrentValue() * 0.01f);
                }
                else
                {
                    applyEqualPowerPan(buffer, 0.f);
                }
                
                buffer.applyGain(juce::Decibels::decibelsToGain(9.f));
            }
            else if (useIR2)
            {
                if (ir2PendingUpdate.exchange(false))
                    irLoader2 = std::move(pendingIR2);
                buffer.applyGain(juce::Decibels::decibelsToGain(ir2LevelParamSmoother.getCurrentValue()));
                juce::dsp::AudioBlock<float> block(buffer);
                irLoader2->process(juce::dsp::ProcessContextReplacing<float>(block));
                
                if (outputIsStereo)
                {
                    applyEqualPowerPan(buffer, ir2PanParamSmoother.getCurrentValue() * 0.01f);
                }
                else
                {
                    applyEqualPowerPan(buffer, 0.f);
                }

                buffer.applyGain(juce::Decibels::decibelsToGain(9.f));
            }
            else if (isIR1Muted && isIR2Muted)
            {
                buffer.applyGain(juce::Decibels::decibelsToGain(-100.f));
                return;
            }


            // Apply EQ to final buffer
            juce::dsp::AudioBlock<float> eqBlock(buffer);
            
            for (int ch {0}; ch < buffer.getNumChannels(); ++ch)
            {
                auto eqBlockCh = eqBlock.getSingleChannelBlock(ch);
                juce::dsp::ProcessContextReplacing<float> eqContextCh(eqBlockCh);
                irEQMonoChainArray[ch].process(eqContextCh);
            }
        }
        
        
        //========================    TONE STACK part    ========================
        if (eqBypassParam->get() == false)
        {
            juce::dsp::AudioBlock<float> toneStackBlock(buffer);

            for (int ch {0}; ch < buffer.getNumChannels(); ++ch)
            {
                auto toneBlock = toneStackBlock.getSingleChannelBlock(ch);
                juce::dsp::ProcessContextReplacing<float> context(toneBlock);
                toneStackMonoChainAray[ch].process(context);
            }
        }
        
        //========================    SATURATION part    ========================
        
        auto drive = saturationDriveParamSmoother.getCurrentValue();

        if (!saturationBypassParam->get() && drive > 0.f)
        {
            float mix = (saturationMixParamSmoother.getCurrentValue()) * 0.01f;
            saturationInstance.processBlock(buffer, drive, saturationModeParam->getIndex(), mix);
        }

        
        //========================    DELAY part    ========================

        if (!delayBypassParam->get())
        {
            bool delayIsMono = !outputIsStereo;
            delayInstance.setFeedback(delayFeedbackParamSmoother.getCurrentValue());
            delayInstance.setMix(delayMixParamSmoother.getCurrentValue());
            using Mode = DelayProcessor::Mode;
            delayInstance.setMode(delayModeParam->getIndex() == 0 ? Mode::Digital : Mode::Tape);
            bool isSync = delaySyncParam->get();
            delayInstance.setSyncEnabled(isSync);
            if (isSync)
                delayInstance.setSubdivision(delayNoteParam->getIndex());
            else
                delayInstance.setDelayTime(delayTimeParamSmoother.getCurrentValue());
            delayInstance.setHostBpm(getPlayHead()->getPosition()->getBpm().orFallback(120.0));
            delayInstance.process(buffer, buffer.getNumSamples(), delayIsMono);
        }


        //========================    OUTPUT GAIN part    ========================
        outputGain.setGainDecibels(outputGainParamSmoother.getCurrentValue());
        applyGain(buffer, outputGain);
        
        //========================    OUTPUT CLIP DETECTION    ========================
        if (clipDetection(buffer))
            clipFlagOut.store(true); // atomic flag to be safe for GUI thread
    }
}

//==============================================================================
bool IRFxAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* IRFxAudioProcessor::createEditor()
{
    return new IRFxAudioProcessorEditor (*this);
}

//==============================================================================
void IRFxAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    auto state = apvts.copyState();
    state.setProperty("CurrentPresetName", currentPresetName, nullptr);
    std::unique_ptr<juce::XmlElement> xml (state.createXml());
    copyXmlToBinary(*xml, destData);
}


void IRFxAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> xml (getXmlFromBinary(data, sizeInBytes));
    if (xml && xml->hasTagName(apvts.state.getType()))
    {
        juce::ValueTree state = juce::ValueTree::fromXml(*xml);
        apvts.replaceState(state);

        // Just cache file path, don't try to load yet!
        if (auto* ir1Path = apvts.state.getPropertyPointer("IR1FilePath"))
            deferredIR1File = juce::File(*ir1Path);
        
        if (auto* ir2Path = apvts.state.getPropertyPointer("IR2FilePath"))
            deferredIR2File = juce::File(*ir2Path);
        
        if (auto* presetNameProp = state.getPropertyPointer("CurrentPresetName"))
            currentPresetName = presetNameProp->toString(); // restore preset name
    }
}

void IRFxAudioProcessor::savePreset(const juce::File& file)
{
    auto state = apvts.copyState();
    std::unique_ptr<juce::XmlElement> xml (state.createXml());
    if (xml) { xml->writeTo(file);}
}

void IRFxAudioProcessor::loadPreset(const juce::File& file)
{
    std::unique_ptr<juce::XmlElement> xml (juce::XmlDocument::parse(file));
    if (xml && xml->hasTagName(apvts.state.getType()))
    {
        juce::ValueTree state = juce::ValueTree::fromXml(*xml);
        if (auto* presetNameProp = state.getPropertyPointer("CurrentPresetName"))
            currentPresetName = presetNameProp->toString(); // restore preset name
        apvts.replaceState(state);
        updateParams();
    }
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new IRFxAudioProcessor();
}
