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
#include "GUI/HorizontalSlider.h"
#include "GUI/GainSlider.h"
#include "ParamNames.h"


//==============================================================================
/**
*/

class IRFxAudioProcessorEditor  : public juce::AudioProcessorEditor, private juce::Timer
{
public:
    IRFxAudioProcessorEditor (IRFxAudioProcessor&);
    ~IRFxAudioProcessorEditor() override;

    
    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;
    void loadIRFile(int);
    void clipLight(juce::Graphics& g);
    void setSaturationType (int);
    

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    IRFxAudioProcessor& audioProcessor;
    const int versionHint = 1;
    void timerCallback() override;
    
    juce::GroupComponent IRGroup, EQGroup, distGroup, delayGroup;
    std::vector<juce::GroupComponent*> groupComponents
    {
        &IRGroup, &EQGroup, &distGroup, &delayGroup
    };
    
//    IR Loader Buttons
    juce::TextButton irLoader1Button{"Load IR1"};
    juce::TextButton irLoader2Button{"Load IR2"};
    std::vector<juce::TextButton*> irLoaderButtons
    {
        &irLoader1Button, &irLoader2Button
    };
    
    std::unique_ptr<juce::File> loadedIRFile1 {nullptr}, loadedIRFile2 {nullptr};
    std::unique_ptr<juce::FileChooser> fileChooser;
    
    HorizontalSlider ir1LevelSlider {"IR1 Level", audioProcessor.apvts, juce::ParameterID(ParamNames::getIR1LevelName(), versionHint), " dB"};
    HorizontalSlider ir2LevelSlider {"IR2 Level", audioProcessor.apvts, juce::ParameterID(ParamNames::getIR2LevelName(), versionHint), " dB"};
    
    void restoreLoadedIRFiles();
    
//    knobImage Dials
    const juce::Image knobImage {juce::ImageCache::getFromMemory(BinaryData::Knob_10brighter_png, BinaryData::Knob_10brighter_pngSize)};
    
    ImageKnob lowCutSlider{"LowCut", audioProcessor.apvts, juce::ParameterID(ParamNames::getIRLowCutName(), versionHint), knobImage, " Hz", 80};
    ImageKnob highCutSlider{"HighCut", audioProcessor.apvts, juce::ParameterID(ParamNames::getIRHighCutName(), versionHint), knobImage, " Hz", 80};
    
    ImageKnob lowEQGainSlider{"Bass", audioProcessor.apvts, juce::ParameterID(ParamNames::getEQLowGainName(), versionHint), knobImage, " dB", 80, true};
    ImageKnob midEQGainSlider{"Mid", audioProcessor.apvts, juce::ParameterID(ParamNames::getEQMidGainName(), versionHint), knobImage, " dB", 80, true};
    ImageKnob midEQFreqSlider{"Mid Freq", audioProcessor.apvts, juce::ParameterID(ParamNames::getEQMidFreqName(), versionHint), knobImage, " Hz", 80, false};
    ImageKnob highEQGainSlider{"Treble", audioProcessor.apvts, juce::ParameterID(ParamNames::getEQHighGainName(), versionHint), knobImage, " dB", 80, true};
       
    ImageKnob saturationKnob{"Drive", audioProcessor.apvts, juce::ParameterID(ParamNames::getDistDriveName(), versionHint), knobImage, "", 110};
    ImageKnob saturationMixKnob{"Mix", audioProcessor.apvts, juce::ParameterID(ParamNames::getDistMixName(), versionHint), knobImage, " %", 110};
    
//    Dials Name Labels
    juce::Label lowCutSliderLabel {"Low Cut"};
    juce::Label highCutSliderLabel {"High Cut"};
    juce::Label lowEQGainSliderLabel {"Bass"};
    juce::Label midEQGainSliderLabel {"Mid"};
    juce::Label midEQFreqSliderLabel {"Mid Freq"};
    juce::Label highEQGainSliderLabel {"Treble"};
    juce::Label saturationLabel {"Drive"};
    juce::Label saturationMixLabel {"Mix"};
    juce::Label satModeLabel {"Mode"};
    std::vector <juce::Label*> dialLabels
    {
        &lowCutSliderLabel,
        &highCutSliderLabel,
        &lowEQGainSliderLabel,
        &midEQGainSliderLabel,
        &midEQFreqSliderLabel,
        &highEQGainSliderLabel,
        &saturationLabel,
        &saturationMixLabel,
        &satModeLabel,
    };
    
//   Bypass Buttons
    BypassButton irBypassButton{"IR Bypass Button", audioProcessor.apvts, juce::ParameterID(ParamNames::getIRBypassName(),versionHint)};
    BypassButton eqBypassButton{"EQ Bypass Button", audioProcessor.apvts, juce::ParameterID(ParamNames::getEQBypassName(),versionHint)};
    BypassButton distBypassButton{"Dist Bypass Button", audioProcessor.apvts, juce::ParameterID(ParamNames::getDistBypassName(),versionHint)};
    BypassButton delayBypassButton{"Delay Bypass Button", audioProcessor.apvts, juce::ParameterID(ParamNames::getDelayBypassName(),versionHint)};
    
//  Unload IR Buttons
    juce::ImageButton unloadIR1Button, unloadIR2Button;
    std::vector<juce::ImageButton*> unloadIRButtons
    {
        &unloadIR1Button, &unloadIR2Button,
    };
    juce::Image unloadIRImage = juce::ImageCache::getFromMemory(BinaryData::CancelX_png, BinaryData::CancelX_pngSize);
//    Mute IR Buttons
    juce::ImageButton muteIR1Button, muteIR2Button;
    std::vector<juce::ImageButton*> muteIRButtons
    {
        &muteIR1Button, &muteIR2Button
    };
    juce::Image muteIRImage = juce::ImageCache::getFromMemory(BinaryData::Mute_png, BinaryData::Mute_pngSize);
    
    
//   SATURATION TYPE BUTTONS
    juce::TextButton sat1Button {"1"}, sat2Button {"2"}, sat3Button{"3"};
    std::vector<juce::TextButton*> satModeButtons
    {
        &sat1Button, &sat2Button, &sat3Button
    };
    
    void setSatButtonColour (juce::TextButton& b, bool isOn);
    
    
//    INPUT & OUTPUT GAIN SLIDERS
    GainSlider inputGainSlider {"Input Gain", audioProcessor.apvts, juce::ParameterID(ParamNames::getInGainName(), versionHint), " dB [IN]", juce::Slider::TextEntryBoxPosition::TextBoxRight};
    GainSlider outputGainSlider {"Output Gain", audioProcessor.apvts, juce::ParameterID(ParamNames::getOutGainName(), versionHint), " dB [OUT]", juce::Slider::TextEntryBoxPosition::TextBoxLeft};
    bool isClippingLightOn = false;
    int clipLightHoldCounter = 0; // counts down in timer ticks
    float clipPopScale = 1.0f;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (IRFxAudioProcessorEditor)
};
