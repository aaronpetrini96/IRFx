/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "GUI/ImageKnob.h"
#include "GUI/BypassButton.h"
#include "ParamNames.h"


//==============================================================================
/**
*/

class IRFxAudioProcessorEditor  : public juce::AudioProcessorEditor
{
public:
    IRFxAudioProcessorEditor (IRFxAudioProcessor&);
    ~IRFxAudioProcessorEditor() override;

    
    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;
    void loadIRFile(int);
    

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    IRFxAudioProcessor& audioProcessor;
    
    juce::GroupComponent IRGroup, EQGroup, distGroup, delayGroup;
    std::vector<juce::GroupComponent*> groupComponents
    {
        &IRGroup, &EQGroup, &distGroup, &delayGroup
    };
    
//    IR Loader Buttons
    juce::TextButton irLoader1{"Load IR1"};
    juce::TextButton irLoader2{"Load IR2"};
    std::vector<juce::TextButton*> irLoaderButtons
    {
        &irLoader1, &irLoader2
    };
    
    std::unique_ptr<juce::File> loadedIRFile1 {nullptr}, loadedIRFile2 {nullptr};
    std::unique_ptr<juce::FileChooser> fileChooser;
    
    void restoreLoadedIRFiles();
    
//    knobImage Dials
    const juce::Image knobImage {juce::ImageCache::getFromMemory(BinaryData::Knob_10brighter_png, BinaryData::Knob_10brighter_pngSize)};
    const int versionHint = 1;
    
    ImageKnob lowCutSlider{"LowCut", audioProcessor.apvts, juce::ParameterID(ParamNames::getIRLowCutName(), versionHint), knobImage, " Hz"};
    ImageKnob highCutSlider{"HighCut", audioProcessor.apvts, juce::ParameterID(ParamNames::getIRHighCutName(), versionHint), knobImage, " Hz"};
    
    ImageKnob lowEQGainSlider{"Bass", audioProcessor.apvts, juce::ParameterID(ParamNames::getEQLowGainName(), versionHint), knobImage, " dB", true};
    ImageKnob midEQGainSlider{"Mid", audioProcessor.apvts, juce::ParameterID(ParamNames::getEQMidGainName(), versionHint), knobImage, " dB", true};
    ImageKnob midEQFreqSlider{"Mid Freq", audioProcessor.apvts, juce::ParameterID(ParamNames::getEQMidFreqName(), versionHint), knobImage, " Hz", false};
    ImageKnob highEQGainSlider{"Treble", audioProcessor.apvts, juce::ParameterID(ParamNames::getEQHighGainName(), versionHint), knobImage, " dB", true};
    
//    Dials Name Labels
    juce::Label lowCutSliderLabel {"Low Cut"};
    juce::Label highCutSliderLabel {"High Cut"};
    juce::Label lowEQGainSliderLabel {"Bass"};
    juce::Label midEQGainSliderLabel {"Mid"};
    juce::Label midEQFreqSliderLabel {"Mid Freq"};
    juce::Label highEQGainSliderLabel {"Treble"};
    std::vector <juce::Label*> dialLabels
    {
        &lowCutSliderLabel,
        &highCutSliderLabel,
        &lowEQGainSliderLabel,
        &midEQGainSliderLabel,
        &midEQFreqSliderLabel,
        &highEQGainSliderLabel
    };
    
//   Bypass Buttons
    BypassButton irBypassButton{"IR Bypass Button", audioProcessor.apvts, juce::ParameterID(ParamNames::getIRBypassName(),versionHint)};
    BypassButton eqBypassButton{"EQ Bypass Button", audioProcessor.apvts, juce::ParameterID(ParamNames::getEQBypassName(),versionHint)};
    BypassButton distBypassButton{"Dist Bypass Button", audioProcessor.apvts, juce::ParameterID(ParamNames::getDistBypassName(),versionHint)};
    BypassButton delayBypassButton{"Delay Bypass Button", audioProcessor.apvts, juce::ParameterID(ParamNames::getDelayBypassName(),versionHint)};

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (IRFxAudioProcessorEditor)
};
