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
    };
    
    initCachedParams<juce::AudioParameterBool*>(boolParams, boolNameFuncs);
    
    //============ CHOICE PARAMS ============
    auto choiceParams = std::array
    {
//        DIST
        &saturationModeParam,
    };
    
    auto choiceNameFuncs = std::array
    {
        &ParamNames::getDistModeName,
    };
    
//    initCachedParams<juce::AudioParameterChoice*>(choiceParams, choiceNameFuncs);
    for (size_t i = 0 ; i < choiceParams.size(); ++i)
    {
        auto ptrToParamPtr = choiceParams [i];
        *ptrToParamPtr = dynamic_cast<juce::AudioParameterChoice*>(apvts.getParameter(choiceNameFuncs[i]()));
        jassert(*ptrToParamPtr != nullptr);
    }
    
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
    params.emplace_back(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID(name, versionHint), name, juce::NormalisableRange<float>(20.f, 1000.f, 1.f, 1.f), 20.f));
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
    
    
//   IN-OUT GAIN
    name = ParamNames::getInGainName();
    params.emplace_back(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID(name, versionHint), name, juce::NormalisableRange<float>(-60.f, 12.f, 0.1f, 1.f), 0.f));
    name = ParamNames::getOutGainName();
    params.emplace_back(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID(name, versionHint), name, juce::NormalisableRange<float>(-60.f, 12.f, 0.1f, 1.f), 0.f));

    
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
    
    irLoader1.reset();
    irLoader2.reset();
    irLoader1.prepare(spec);
    irLoader2.prepare(spec);
    
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
    
//    saturationPreEQ.state = Coefficients::makeLowShelf(sampleRate, 150.f, 0.707f, juce::Decibels::decibelsToGain(2.f));
//    saturationPostEQ.state = Coefficients::makeLowPass(sampleRate, 15000.f);
//    saturationPreEQ.prepare(spec);
//    saturationPostEQ.prepare(spec);
    saturationInstance.prepare(spec);
    
    
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
    irLoader1.loadImpulseResponse(irFile, juce::dsp::Convolution::Stereo::yes, juce::dsp::Convolution::Trim::yes, 0, juce::dsp::Convolution::Normalise::yes);
    isIR1Loaded = true;
    apvts.state.setProperty("IR1FilePath", irFile.getFullPathName(), nullptr);
}

void IRFxAudioProcessor::loadIR2(const juce::File &irFile)
{
    irLoader2.loadImpulseResponse(irFile, juce::dsp::Convolution::Stereo::yes, juce::dsp::Convolution::Trim::yes, 0, juce::dsp::Convolution::Normalise::yes);
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

//float IRFxAudioProcessor::neveStyleSaturation (float x, float drive)
//{
////    Apply Drive
////    x *= drive;
//    float mappedDrive = juce::jmap(drive, 0.0f, 12.0f, 1.0f, 10.0f);
//    x *= mappedDrive;
//    
//    
//
//    float asym = 0.1f * x * x * x;
//    float saturated = std::tanh(x + asym);
//    saturated *= 0.8f;
//
//    return saturated;
//}
//float IRFxAudioProcessor::sslStyleSaturation (float x, float drive)
//{
//    //    Apply Drive
//    //    x *= drive;
//    float mappedDrive = juce::jmap(drive, 0.0f, 12.0f, 1.0f, 10.0f);
//    x *= mappedDrive;
//    
//    float saturated = std::tanh(x);
//    saturated *= 0.7f; // attenuation, adjust to taste
//    return saturated;
//}
//
//float IRFxAudioProcessor::apiStyleSaturation (float x, float drive)
//{
//    //    Apply Drive
//    //    x *= drive;
//    float mappedDrive = juce::jmap(drive, 0.0f, 12.0f, 1.0f, 10.0f);
//    x *= mappedDrive;
//
//    // Stronger asymmetry, more "bite"
//    float asym = 0.3f * x * x * x;
//    float saturated = std::tanh(x + asym);
//    
//    saturated *= 0.7f; // adjust to keep level in check
//    return saturated;
//}

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
    
    juce::dsp::AudioBlock<float> block(buffer);
    juce::AudioBuffer<float> tempBuffer;
    
    
    //========================    IN GAIN part    ========================
    juce::AudioBuffer<float> dryBuffer;
    dryBuffer.makeCopyOf(buffer);
    inputGain.setGainDecibels(inputGainParamSmoother.getCurrentValue());
    
    inputLevelL = buffer.getRMSLevel(0, 0, buffer.getNumSamples());
    inputLevelR = buffer.getNumChannels() > 1 ? buffer.getRMSLevel(1, 0, buffer.getNumSamples()) : inputLevelL;
    //========================                    ========================
    
    updateSmootherFromParams(buffer.getNumSamples(), SmootherUpdateMode::liveInRealTime);
    updateParams();
    
    applyGain(buffer, inputGain);
    
    if (irLoaderBypassParam->get() == false)
    {
        // Evaluate effective loading states after mute
        const bool useIR1 = isIR1Loaded && !isIR1Muted;
        const bool useIR2 = isIR2Loaded && !isIR2Muted;

        if (useIR1 && useIR2)
        {
            tempBuffer.setSize(buffer.getNumChannels(), buffer.getNumSamples());
            tempBuffer.makeCopyOf(buffer, true);
            
            buffer.applyGain(juce::Decibels::decibelsToGain(ir1LevelParamSmoother.getCurrentValue()));
            tempBuffer.applyGain(juce::Decibels::decibelsToGain(ir2LevelParamSmoother.getCurrentValue()));
            
            juce::dsp::AudioBlock<float> block1(buffer);
            juce::dsp::AudioBlock<float> block2(tempBuffer);
            

            irLoader1.process(juce::dsp::ProcessContextReplacing<float>(block1));
            irLoader2.process(juce::dsp::ProcessContextReplacing<float>(block2));

            for (int ch = 0; ch < buffer.getNumChannels(); ++ch)
                buffer.addFrom(ch, 0, tempBuffer, ch, 0, buffer.getNumSamples());

            buffer.applyGain(juce::Decibels::decibelsToGain(3.f));
        }
        else if (useIR1)
        {
            buffer.applyGain(juce::Decibels::decibelsToGain(ir1LevelParamSmoother.getCurrentValue()));
            juce::dsp::AudioBlock<float> block(buffer);
            irLoader1.process(juce::dsp::ProcessContextReplacing<float>(block));

            buffer.applyGain(juce::Decibels::decibelsToGain(9.f));
        }
        else if (useIR2)
        {
            buffer.applyGain(juce::Decibels::decibelsToGain(ir2LevelParamSmoother.getCurrentValue()));
            juce::dsp::AudioBlock<float> block(buffer);
            irLoader2.process(juce::dsp::ProcessContextReplacing<float>(block));

            buffer.applyGain(juce::Decibels::decibelsToGain(9.f));
        }

        // Apply EQ to final buffer
        juce::dsp::AudioBlock<float> eqBlock(buffer);
        auto eqBlockLeft = eqBlock.getSingleChannelBlock(0);
        auto eqBlockRight = eqBlock.getSingleChannelBlock(1);

        juce::dsp::ProcessContextReplacing<float> eqContextLeft(eqBlockLeft);
        juce::dsp::ProcessContextReplacing<float> eqContextRight(eqBlockRight);

        irEQMonoChainArray[0].process(eqContextLeft);
        irEQMonoChainArray[1].process(eqContextRight);
    }
    
    
    if (eqBypassParam->get() == false)
    {
        juce::dsp::AudioBlock<float> toneStackBlock(buffer);
        auto toneStackBlockLeft = toneStackBlock.getSingleChannelBlock(0);
        auto toneStackBlockRight = toneStackBlock.getSingleChannelBlock(1);
        
        juce::dsp::ProcessContextReplacing<float> toneStackContextLeft (toneStackBlockLeft), toneStackContextRight (toneStackBlockRight);
        
        toneStackMonoChainAray[0].process(toneStackContextLeft);
        toneStackMonoChainAray[1].process(toneStackContextRight);
    }
    
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
    
    outputGain.setGainDecibels(outputGainParamSmoother.getCurrentValue());
    applyGain(buffer, outputGain);
    //    OUTPUT CLIP DETECTION
    if (clipDetection(buffer))
        clipFlagOut.store(true); // atomic flag to be safe for GUI thread
    
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
