/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
IRFxAudioProcessorEditor::IRFxAudioProcessorEditor (IRFxAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    

//    GROUP GENERAL SETTINGS
    IRGroup.setText("IR Loader");
    EQGroup.setText("Tone Stack");
    distGroup.setText("Saturation");
    delayGroup.setText("Delay");
    for (auto group : groupComponents)
    {
        group->setTextLabelPosition(juce::Justification::horizontallyCentred);
        group->setColour(juce::GroupComponent::ColourIds::outlineColourId, juce::Colours::white);
        addAndMakeVisible(*group);
    }
    
    
//    DIAL'S LABELS GENERAL SETUP
    for (auto label : dialLabels)
    {
        label->setText(label->getName(), juce::dontSendNotification);
        label->setJustificationType(juce::Justification::centred);
        label->setSize(80, 30);
        label->setColour(juce::Label::ColourIds::textColourId, juce::Colours::white.darker(0.2f));
    }

    
//    IR Loaders Buttons General setup
    for (auto button : irLoaderButtons)
    {
        button->setSize(150, 35);
        button->setColour(juce::TextButton::ColourIds::textColourOffId, juce::Colours::white.withAlpha(0.5f));
    }

    irLoader1Button.onClick = [this](){loadIRFile(1);};
    irLoader2Button.onClick = [this](){loadIRFile(2);};
    
//    IR UNLOAD BUTTONS
    for(auto button : unloadIRButtons)
    {
        button -> setClickingTogglesState(true);
        button -> setBounds(0, 0, 25, 25);
        button -> setImages(false, true, true,
                            unloadIRImage, 1.f, juce::Colours::red.darker(0.5f),
                            unloadIRImage, 1.f, juce::Colours::red.darker(0.1f),
                            unloadIRImage, 1.f, juce::Colours::red.darker(0.5f),
                            0.f);
        button -> setSize(30, 30);
    }
    unloadIR1Button.onClick = [this]
    {
        if(audioProcessor.isIR1Loaded)
        {
            loadedIRFile1 = nullptr;
            audioProcessor.irLoader1.loadImpulseResponse(juce::AudioBuffer<float>(), audioProcessor.getSampleRate(), juce::dsp::Convolution::Stereo::yes, juce::dsp::Convolution::Trim::no, juce::dsp::Convolution::Normalise::no);
            audioProcessor.isIR1Loaded = false;
            audioProcessor.apvts.state.removeProperty("IR1FilePath", nullptr);
            irLoader1Button.setButtonText("Load IR1");
            irLoader1Button.setColour(juce::TextButton::ColourIds::textColourOffId, juce::Colours::white.withAlpha(0.5f));
            ir1LevelSlider.setVisible(false);
        }
        
    };
    unloadIR2Button.onClick = [this]
    {
        if (audioProcessor.isIR2Loaded)
        {
            loadedIRFile2 = nullptr;
            audioProcessor.irLoader2.loadImpulseResponse(juce::AudioBuffer<float>(), audioProcessor.getSampleRate(), juce::dsp::Convolution::Stereo::yes, juce::dsp::Convolution::Trim::no, juce::dsp::Convolution::Normalise::no);
            audioProcessor.isIR2Loaded = false;
            audioProcessor.apvts.state.removeProperty("IR2FilePath", nullptr);
            irLoader2Button.setButtonText("Load IR2");
            irLoader2Button.setColour(juce::TextButton::ColourIds::textColourOffId, juce::Colours::white.withAlpha(0.5f));
            ir2LevelSlider.setVisible(false);
        }
    };
    
//    IR MUTE BUTTONS
    for(auto button : muteIRButtons)
    {
        button -> setClickingTogglesState(true);
        button -> setBounds(0, 0, 30, 30);
        button -> setImages(false, true, true,
                            muteIRImage, 1.f, juce::Colours::white.darker(0.5f),
                            muteIRImage, 1.f, juce::Colours::yellow.darker(0.1f),
                            muteIRImage, 1.f, juce::Colours::yellow.darker(0.5f),
                            0.f);
        button -> setSize(35, 35);
    }
    muteIR1Button.onClick = [this]
    {
        audioProcessor.isIR1Muted = !audioProcessor.isIR1Muted;
    };
    muteIR2Button.onClick = [this]
    {
        audioProcessor.isIR2Muted = !audioProcessor.isIR2Muted;
    };
    


    
//    Add And Make Visible
    
    IRGroup.addAndMakeVisible(irBypassButton);
    IRGroup.addAndMakeVisible(irLoader1Button);
    IRGroup.addAndMakeVisible(irLoader2Button);
    IRGroup.addAndMakeVisible(unloadIR1Button);
    IRGroup.addAndMakeVisible(unloadIR2Button);
    IRGroup.addAndMakeVisible(muteIR1Button);
    IRGroup.addAndMakeVisible(muteIR2Button);
    IRGroup.addAndMakeVisible(ir1LevelSlider);
    IRGroup.addAndMakeVisible(ir2LevelSlider);
    
    IRGroup.addAndMakeVisible(lowCutSlider);
    IRGroup.addAndMakeVisible(lowCutSliderLabel);
    IRGroup.addAndMakeVisible(highCutSlider);
    IRGroup.addAndMakeVisible(highCutSliderLabel);
    
    EQGroup.addAndMakeVisible(eqBypassButton);
    EQGroup.addAndMakeVisible(lowEQGainSlider);
    EQGroup.addAndMakeVisible(lowEQGainSliderLabel);
    EQGroup.addAndMakeVisible(midEQGainSlider);
    EQGroup.addAndMakeVisible(midEQGainSliderLabel);
    EQGroup.addAndMakeVisible(midEQFreqSlider);
    EQGroup.addAndMakeVisible(midEQFreqSliderLabel);
    EQGroup.addAndMakeVisible(highEQGainSlider);
    EQGroup.addAndMakeVisible(highEQGainSliderLabel);
    
    distGroup.addAndMakeVisible(distBypassButton);
    distGroup.addAndMakeVisible(saturationKnob);
    distGroup.addAndMakeVisible(saturationLabel);
    distGroup.addAndMakeVisible(saturationMixKnob);
    distGroup.addAndMakeVisible(saturationMixLabel);
    
    delayGroup.addAndMakeVisible(delayBypassButton);
   
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    ir1LevelSlider.setVisible(false);
    ir2LevelSlider.setVisible(false);
    restoreLoadedIRFiles();
    
    setSize (600, 800);
}

IRFxAudioProcessorEditor::~IRFxAudioProcessorEditor()
{
}

//==============================================================================
void IRFxAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (juce::Colours::black.withAlpha(0.8f));

}

void IRFxAudioProcessorEditor::resized()
{
    auto bounds = getLocalBounds().withSizeKeepingCentre(600, 600);
    auto boundsHeight = bounds.getHeight();
    auto boundsWidth = bounds.getWidth();
    auto y = boundsHeight * 0.015;
    auto height = boundsHeight * 0.5 - y * 2;
    auto leftMargin = boundsWidth * 0.02;
    auto groupWidth = boundsWidth * 0.5 - leftMargin * 1.5;
    auto dialSize = boundsWidth * 0.25;
    auto labelWidth = lowCutSliderLabel.getWidth();
    auto labelHeight = lowCutSliderLabel.getHeight();
    auto bypassButtonSize = boundsWidth * 0.10;
    auto irLoaderButtonWidth = irLoader1Button.getWidth();
    auto irLoaderButtonHeight = irLoader1Button.getHeight();
    auto unloadIRButtonSize = unloadIR1Button.getWidth();
    auto muteIRButtonSize = muteIR1Button.getWidth();
    auto irLevelSliderWidth = ir1LevelSlider.getWidth();
    auto irLevelSliderHeight = ir1LevelSlider.getHeight();
    auto saturationDialSize = boundsWidth * 0.25;

    
    
    IRGroup.setBounds(leftMargin, y, groupWidth, height);
    EQGroup.setBounds(IRGroup.getRight() + leftMargin, IRGroup.getY(), groupWidth, height);
    distGroup.setBounds(leftMargin, IRGroup.getBottom() + y, groupWidth, height);
    delayGroup.setBounds(EQGroup.getX(), distGroup.getY(), groupWidth, height);
    
//    IR GROUP
    irBypassButton.setBounds(IRGroup.getWidth() * 0.9, IRGroup.getHeight() * 0.05, bypassButtonSize, bypassButtonSize);

    irLoader1Button.setBounds(IRGroup.getWidth() * 0.1, IRGroup.getHeight() * 0.16, irLoaderButtonWidth , irLoaderButtonHeight);
    unloadIR1Button.setBounds(irLoader1Button.getRight() * 1.05, irLoader1Button.getY() + (irLoaderButtonHeight - unloadIRButtonSize) * 0.5, unloadIRButtonSize, unloadIRButtonSize);
    muteIR1Button.setBounds(unloadIR1Button.getRight() * 1.05, irLoader1Button.getY() + (irLoaderButtonHeight - muteIRButtonSize) * 0.5, muteIRButtonSize, muteIRButtonSize);
    ir1LevelSlider.setBounds(irLoader1Button.getX(), IRGroup.getHeight() * 0.055, irLevelSliderWidth, irLevelSliderHeight);
    
    irLoader2Button.setBounds(irLoader1Button.getX(), irLoader1Button.getBottom() * 1.1, irLoaderButtonWidth, irLoaderButtonHeight);
    unloadIR2Button.setBounds(unloadIR1Button.getX(), irLoader2Button.getY() + (irLoaderButtonHeight - unloadIRButtonSize) * 0.5, unloadIRButtonSize, unloadIRButtonSize);
    muteIR2Button.setBounds(unloadIR2Button.getRight() * 1.05, irLoader2Button.getY() + (irLoaderButtonHeight - muteIRButtonSize) * 0.5, muteIRButtonSize, muteIRButtonSize);
    ir2LevelSlider.setBounds(irLoader2Button.getX(), irLoader2Button.getY() * 1.4, irLevelSliderWidth, irLevelSliderHeight);
    
    lowCutSlider.setBounds(IRGroup.getWidth() * 0.115, IRGroup.getHeight() * 0.65, dialSize, dialSize);
    lowCutSliderLabel.setBounds(lowCutSlider.getX(), lowCutSlider.getY() * 0.85, labelWidth, labelHeight);
    highCutSlider.setBounds(IRGroup.getRight() - dialSize * 0.83, lowCutSlider.getY(), dialSize, dialSize);
    highCutSliderLabel.setBounds(highCutSlider.getX(), highCutSlider.getY() * 0.85, labelWidth, labelHeight);
    
//    EQ GROUP
    eqBypassButton.setBounds(EQGroup.getWidth() * 0.9, EQGroup.getHeight() * 0.05, bypassButtonSize, bypassButtonSize);
    lowEQGainSlider.setBounds(EQGroup.getWidth() * 0.1, EQGroup.getHeight() * 0.2, dialSize, dialSize);
    lowEQGainSliderLabel.setBounds(lowEQGainSlider.getX(), lowEQGainSlider.getY() * 0.5, labelWidth, labelHeight);
    midEQGainSlider.setBounds(lowEQGainSlider.getX(), EQGroup.getHeight() * 0.65, dialSize, dialSize);
    midEQGainSliderLabel.setBounds(midEQGainSlider.getX(), midEQGainSlider.getY() * 0.85, labelWidth, labelHeight);
    highEQGainSlider.setBounds(EQGroup.getWidth() * 0.6, lowEQGainSlider.getY(), dialSize, dialSize);
    highEQGainSliderLabel.setBounds(highEQGainSlider.getX(), lowEQGainSliderLabel.getY(), labelWidth, labelHeight);
    midEQFreqSlider.setBounds(highEQGainSlider.getX(), midEQGainSlider.getY(), dialSize, dialSize);
    midEQFreqSliderLabel.setBounds(midEQFreqSlider.getX(), midEQGainSliderLabel.getY(), labelWidth, labelHeight);
    
//    DIST GROUP
    distBypassButton.setBounds(distGroup.getWidth() * 0.9, distGroup.getHeight() * 0.05, bypassButtonSize, bypassButtonSize);
    saturationKnob.setBounds(distGroup.getWidth() * 0.55, distGroup.getHeight() * 0.3, saturationDialSize, saturationDialSize);
    saturationLabel.setBounds(saturationKnob.getX() * 1.09, saturationKnob.getY() * 0.7, labelWidth, labelHeight);
    saturationMixKnob.setBounds(distGroup.getWidth() * 0.07, saturationKnob.getY(), saturationDialSize, saturationDialSize);
    saturationMixLabel.setBounds(saturationMixKnob.getX() * 1.7, saturationLabel.getY(), labelWidth, labelHeight);
    
//    DELAY GROUP
    delayBypassButton.setBounds(delayGroup.getWidth() * 0.9, delayGroup.getHeight() * 0.05, bypassButtonSize, bypassButtonSize);

}



void IRFxAudioProcessorEditor::loadIRFile(int irIndex)
{
    fileChooser = std::make_unique<juce::FileChooser> ("Select IR", juce::File::getSpecialLocation(juce::File::userDesktopDirectory), "*.wav");

    auto fileChooserFlags = juce::FileBrowserComponent::openMode | juce::FileBrowserComponent::canSelectFiles;

    fileChooser->launchAsync (fileChooserFlags, [this, irIndex] (const juce::FileChooser& fileChooser)
    {
        juce::File selectedFile = fileChooser.getResult();
        
        if (selectedFile.existsAsFile())
        {
            if (irIndex == 1)
            {
                loadedIRFile1 = std::make_unique<juce::File>(selectedFile);
                audioProcessor.loadIR1(*loadedIRFile1);
                irLoader1Button.setButtonText(loadedIRFile1->getFileName());
                irLoader1Button.setColour(juce::TextButton::ColourIds::textColourOffId, juce::Colours::white);
                ir1LevelSlider.setVisible(true);
            }
            else if (irIndex == 2)
            {
                loadedIRFile2 = std::make_unique<juce::File>(selectedFile);
                audioProcessor.loadIR2(*loadedIRFile2);
                irLoader2Button.setButtonText(loadedIRFile2->getFileName());
                irLoader2Button.setColour(juce::TextButton::ColourIds::textColourOffId, juce::Colours::white);
                ir2LevelSlider.setVisible(true);
            }
        }
        else
        {
            juce::AlertWindow::showMessageBoxAsync(juce::MessageBoxIconType::WarningIcon, "Error", "File Could Not Be Loaded");
        }
    });
}

void IRFxAudioProcessorEditor::restoreLoadedIRFiles()
{
    auto ir1Path = audioProcessor.apvts.state.getProperty("IR1FilePath", "").toString();
    if (ir1Path.isNotEmpty())
    {
        juce::File file(ir1Path);
        if (file.existsAsFile())
        {
            loadedIRFile1 = std::make_unique<juce::File>(file);
            irLoader1Button.setButtonText(file.getFileName());
            irLoader1Button.setColour(juce::TextButton::ColourIds::textColourOffId, juce::Colours::white);
            ir1LevelSlider.setVisible(true);
        }
    }

    auto ir2Path = audioProcessor.apvts.state.getProperty("IR2FilePath", "").toString();
    if (ir2Path.isNotEmpty())
    {
        juce::File file(ir2Path);
        if (file.existsAsFile())
        {
            loadedIRFile2 = std::make_unique<juce::File>(file);
            irLoader2Button.setButtonText(file.getFileName());
            irLoader2Button.setColour(juce::TextButton::ColourIds::textColourOffId, juce::Colours::white);
            ir2LevelSlider.setVisible(true);
        }
    }
}
