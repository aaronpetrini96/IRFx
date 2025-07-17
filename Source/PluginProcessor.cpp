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
        &delayMonoStereoParam,
    };
    
    auto choiceNameFuncs = std::array
    {
        &ParamNames::getDistModeName,
        &ParamNames::getDelayModeName,
        &ParamNames::getDelayNoteName,
        &ParamNames::getDelayMonoStereoName,
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
    
//   EQ
    name = ParamNames::getEQBypassName();
    params.emplace_back(std::make_unique<juce::AudioParameterBool>(juce::ParameterID(name, versionHint), name, false));
    name = ParamNames::getEQLowGainName();
    params.emplace_back(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID(name, versionHint), name, juce::NormalisableRange<float>(-24.f, 24.f, 0.1f, 1.f), 0.f));
    name = ParamNames::getEQMidGainName();
    params.emplace_back(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID(name, versionHint), name, juce::NormalisableRange<float>(-24.f, 24.f, 0.1f, 1.f), 0.f));
    name = ParamNames::getEQMidFreqName();
    params.emplace_back(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID(name, versionHint), name, juce::NormalisableRange<float>(250.f, 5000.f, 1.f, 1.f), 550.f));
    name = ParamNames::getEQHighGainName();
    params.emplace_back(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID(name, versionHint), name, juce::NormalisableRange<float>(-24.f, 24.f, 0.1f, 1.f), 0.f));
    
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
    name = ParamNames::getDelayMonoStereoName();
    juce::StringArray delayMonoStereoArray {"Mono", "Ping-Pong"};
    params.emplace_back(std::make_unique<juce::AudioParameterChoice>(juce::ParameterID(name, versionHint), name, delayMonoStereoArray, 0));
    
//   IN-OUT GAIN
    name = ParamNames::getInGainName();
    params.emplace_back(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID(name, versionHint), name, juce::NormalisableRange<float>(-60.f, 12.f, 0.1f, 1.f), 0.f));
    name = ParamNames::getOutGainName();
    params.emplace_back(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID(name, versionHint), name, juce::NormalisableRange<float>(-60.f, 12.f, 0.1f, 1.f), 0.f));
    
    
//   PLUGIN BYPASS
    name = ParamNames::getGeneralBypassName();
    params.emplace_back(std::make_unique<juce::AudioParameterBool>(juce::ParameterID(name, versionHint), name, false));

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
    
//    SATURATION

    
//    DELAY
    
}
//==============================================================================
//==============================================================================
void IRFxAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    spec.maximumBlockSize = samplesPerBlock;
    spec.sampleRate = sampleRate;
    spec.numChannels = getTotalNumOutputChannels();
    
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
}

void IRFxAudioProcessor::updateSmootherFromParams(int numSamplesToSkip, SmootherUpdateMode init)
{
    auto paramsNeedingSmoothing = std::vector
    {
        lowCutFreqParam,
        highCutFreqParam,
        ir1LevelParam,
        ir2LevelParam,
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

#ifndef JucePlugin_PreferredChannelConfigurations
bool IRFxAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif


void IRFxAudioProcessor::loadIR1(const juce::File &irFile)
{
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
    
    apvts.state.setProperty("IR1FilePath", irFile.getFullPathName(), nullptr);
}


void IRFxAudioProcessor::loadIR2(const juce::File& irFile)
{
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

    apvts.state.setProperty("IR2FilePath", irFile.getFullPathName(), nullptr);
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
// CLIP DETECTION
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
    [[maybe_unused]] auto totalNumInputChannels  = getTotalNumInputChannels();
    [[maybe_unused]] auto totalNumOutputChannels = getTotalNumOutputChannels();
    


    //========================    IN GAIN part    ========================
    juce::AudioBuffer<float> dryBuffer;
    dryBuffer.makeCopyOf(buffer);
    inputGain.setGainDecibels(inputGainParamSmoother.getCurrentValue());
    
    inputLevelL = buffer.getRMSLevel(0, 0, buffer.getNumSamples());
    inputLevelR = buffer.getNumChannels() > 1 ? buffer.getRMSLevel(1, 0, buffer.getNumSamples()) : inputLevelL;
    //========================                    ========================
    
    updateSmootherFromParams(buffer.getNumSamples(), SmootherUpdateMode::liveInRealTime);
    updateParams();
    

    if (pluginBypassParam->get() == false)
    {
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
                buffer.applyGain(juce::Decibels::decibelsToGain(9.f));
            }
            else if (useIR2)
            {
                if (ir2PendingUpdate.exchange(false))
                    irLoader2 = std::move(pendingIR2);
                buffer.applyGain(juce::Decibels::decibelsToGain(ir2LevelParamSmoother.getCurrentValue()));
                juce::dsp::AudioBlock<float> block(buffer);
                irLoader2->process(juce::dsp::ProcessContextReplacing<float>(block));
                buffer.applyGain(juce::Decibels::decibelsToGain(9.f));
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
        /*
        if (!saturationBypassParam->get() && drive > 0.f )
        {
            juce::dsp::AudioBlock<float> saturationBlock(buffer);
            juce::dsp::ProcessContextReplacing<float> preSaturationContext(saturationBlock);
            saturationPreEQ.process(preSaturationContext);
            
            float mix = (saturationMixParamSmoother.getCurrentValue()) * 0.01f;
            
            std::vector<float> dryBuffer(block.getNumSamples()), wetBuffer(block.getNumSamples());
            
            for (size_t ch {0}; ch < saturationBlock.getNumChannels(); ++ch)
            {
                auto* samples = block.getChannelPointer(ch);
                
                for (size_t i{0}; i < block.getNumSamples(); ++i)
                {
                    float dry = samples [i];
                    dryBuffer[i] = dry;
                    switch (saturationModeParam->getIndex())
                    {
                        case 0:
                            wetBuffer[i] = neveStyleSaturation(dry, drive);
                            break;
                        case 1:
                            wetBuffer[i] = sslStyleSaturation(dry, drive);
                            break;
                        case 2:
                            wetBuffer[i] = apiStyleSaturation(dry, drive);
                            break;
                    }
                }
                
                float dryRMS = computeRMS(dryBuffer.data(), block.getNumSamples());
                float wetRMS = computeRMS(wetBuffer.data(), block.getNumSamples());
                float targetRMS = 0.1f;
                static float smoothedGain = 1.f;
                static bool gainInitialized = false;
                
                float makeUpGain = (wetRMS > 0.0001f) ? targetRMS/wetRMS : 1.f;
                
                if (!gainInitialized)
                {
                    smoothedGain = makeUpGain * 0.3f;
                    gainInitialized = true;
                }
                else
                {
                    float smoothingCoeff = 0.99f;
                    smoothedGain = smoothedGain * smoothingCoeff + makeUpGain * (1.f - smoothingCoeff);
                }
                if (dryRMS < 0.0001f)
                {
                    gainInitialized = false;
                    smoothedGain = 1.0f;
                }

                for (size_t i{0}; i < block.getNumSamples(); ++i)
                {
                    wetBuffer[i] *= smoothedGain;
                    samples[i] = dryBuffer[i] * (1.0f - mix) + wetBuffer[i] * mix;
                }
            }
            juce::dsp::ProcessContextReplacing<float> postSaturationContext(saturationBlock);
            saturationPostEQ.process(postSaturationContext);
        }
         */
        
        //========================    DELAY part    ========================
        if (!delayBypassParam->get())
        {
//            // Make sure buffer is stereo for ping-pong
//            if (buffer.getNumChannels() < 2)
//            {
//                buffer.setSize(2, buffer.getNumSamples(), true, true, true);
//                buffer.copyFrom(1, 0, buffer, 0, 0, buffer.getNumSamples());
//            }
            
            delayInstance.setFeedback(delayFeedbackParamSmoother.getCurrentValue());
            delayInstance.setMix(delayMixParamSmoother.getCurrentValue());
            delayInstance.setMode(delayModeParam->getIndex() == 0 ? DelayProcessor::Mode::Digital : DelayProcessor::Mode::Tape);
            bool isSync = delaySyncParam->get();
            delayInstance.setSyncEnabled(isSync);
            if (isSync)
                delayInstance.setSubdivision(delayNoteParam->getIndex());
            else
                delayInstance.setDelayTime(delayTimeParamSmoother.getCurrentValue());
            
            delayInstance.setHostBpm(getPlayHead()->getPosition()->getBpm().orFallback(120.0));
            
            bool isMono = delayMonoStereoParam->getIndex() == 0 ? true : false;
            delayInstance.process(buffer, buffer.getNumSamples(), isMono);
        }
        
        outputGain.setGainDecibels(outputGainParamSmoother.getCurrentValue());
        applyGain(buffer, outputGain);
        
        
        //========================    OUTPUT CLIP DETECTION    ========================
        if (clipDetection(buffer))
            clipFlagOut.store(true); // atomic flag to be safe for GUI thread
    }

    outputLevelL = buffer.getRMSLevel(0, 0, buffer.getNumSamples());
    outputLevelR = buffer.getNumChannels() > 1 ? buffer.getRMSLevel(1, 0, buffer.getNumSamples()) : outputLevelL;
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
        if (auto* ir1Path = apvts.state.getPropertyPointer("IR1FilePath"))
        {
            juce::File file(*ir1Path);
            if (file.existsAsFile())
                loadIR1(file);
        }

        if (auto* ir2Path = apvts.state.getPropertyPointer("IR2FilePath"))
        {
            juce::File file(*ir2Path);
            if (file.existsAsFile())
                loadIR2(file);
        }
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
        apvts.replaceState(state);
        updateParams();
    }
}

void IRFxAudioProcessor::loadDefaultPreset()
{
    loadPreset(juce::File::getSpecialLocation(juce::File::userDocumentsDirectory).getChildFile("DefaultPreset.xml"));
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new IRFxAudioProcessor();
}
