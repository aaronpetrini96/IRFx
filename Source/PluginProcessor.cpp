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
        
//        EQ
        &lowEQGainParam,
        &midEQGainParam,
        &midEQFreqParam,
        &highEQGainParam,
        
//        SAT
        
//        DELAY
        
    };
    
    auto floatNameFuncs = std::array
    {
        //        IR LOADER
        &ParamNames::getIRLowCutName,
        &ParamNames::getIRHighCutName,
        
        //        EQ
        &ParamNames::getEQLowGainName,
        &ParamNames::getEQMidGainName,
        &ParamNames::getEQMidFreqName,
        &ParamNames::getEQHighGainName,
        
        //        SAT
        
        //        DELAY
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
        
        //        DELAY
    };
    
    auto boolNameFuncs = std::array
    {
        //        IR LOADER
        &ParamNames::getIRBypassName,
        //        EQ
        &ParamNames::getEQBypassName,
        
        //        SAT
        
        //        DELAY
    };
    
    initCachedParams<juce::AudioParameterBool*>(boolParams, boolNameFuncs);
    
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
    
//   DELAY
    name = ParamNames::getDelayBypassName();
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

    for (auto& chain : monoChains)
    {
        auto& lowCut = chain.template get<0>();
        auto& highCut = chain.template get<1>();
        
        *lowCut.coefficients = *lowCutCoefficients;
        *highCut.coefficients = *highCutCoefficients;
    }
    
//    EQ STACK
    
    
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
    
    juce::dsp::ProcessSpec monoSpec = spec;
    monoSpec.numChannels = 1;  // IMPORTANT: mono
    
    for (int i = 0; i < getTotalNumOutputChannels(); ++i)
    {
        monoChains[i].reset();
        monoChains[i].prepare(monoSpec);
    }
   
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
        lowEQGainParam,
        midEQGainParam,
        midEQFreqParam,
        highEQGainParam,
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
        &lowEQGainParamSmoother,
        &midEQGainParamSmoother,
        &midEQFreqParamSmoother,
        &highEQGainParamSmoother,
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

void IRFxAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    [[maybe_unused]] auto totalNumInputChannels  = getTotalNumInputChannels();
    [[maybe_unused]] auto totalNumOutputChannels = getTotalNumOutputChannels();
    
    juce::dsp::AudioBlock<float> block(buffer);
    juce::AudioBuffer<float> tempBuffer;
    tempBuffer.makeCopyOf(buffer);
    
    updateSmootherFromParams(buffer.getNumSamples(), SmootherUpdateMode::liveInRealTime);
    updateParams();
    
//    auto irBypassed = irLoaderBypassParam->get();
    
    if (irLoaderBypassParam->get() == false)
    {
        if (isIR1Loaded && isIR2Loaded)
        {
            juce::dsp::AudioBlock<float> block1 (buffer);
            juce::dsp::AudioBlock<float> block2 (tempBuffer);
            irLoader1.process(juce::dsp::ProcessContextReplacing<float>(block1));
            irLoader2.process(juce::dsp::ProcessContextReplacing<float>(block2));
            
            for (int ch = 0; ch < buffer.getNumChannels(); ++ch)
                buffer.addFrom(ch, 0, tempBuffer, ch, 0, buffer.getNumSamples());
            
            buffer.applyGain(juce::Decibels::decibelsToGain(3.f));
        }
        else if (isIR1Loaded)
        {
            irLoader1.process(juce::dsp::ProcessContextReplacing<float>(block));
            buffer.applyGain(juce::Decibels::decibelsToGain(9.f));
        }
        else if (isIR2Loaded)
        {
            irLoader2.process(juce::dsp::ProcessContextReplacing<float>(block));
            buffer.applyGain(juce::Decibels::decibelsToGain(9.f));
        }

        
        juce::dsp::AudioBlock<float> eqBlock(buffer);
        auto eqBlockLeft = eqBlock.getSingleChannelBlock(0);
        auto eqBlockRight = eqBlock.getSingleChannelBlock(1);
        
        juce::dsp::ProcessContextReplacing<float> eqContextLeft(eqBlockLeft), eqContextRight (eqBlockRight);
        
        monoChains[0].process(eqContextLeft);
        monoChains[1].process(eqContextRight);
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

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new IRFxAudioProcessor();
}
