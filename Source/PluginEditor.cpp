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
        button->setSize(150, 40);
        button->setColour(juce::TextButton::ColourIds::textColourOffId, juce::Colours::white.withAlpha(0.5f));
    }
    restoreLoadedIRFiles();
    irLoader1.onClick = [this](){loadIRFile(1);};
    irLoader2.onClick = [this](){loadIRFile(2);};
    

    IRGroup.addAndMakeVisible(irBypassButton);
    IRGroup.addAndMakeVisible(irLoader1);
    IRGroup.addAndMakeVisible(irLoader2);
    
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
    
    delayGroup.addAndMakeVisible(delayBypassButton);
   
   
    
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
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
    auto y = bounds.getHeight() * 0.015;
    auto height = bounds.getHeight() * 0.5 - y * 2;
    auto leftMargin = bounds.getWidth() * 0.02;
    auto groupWidth = bounds.getWidth() * 0.5 - leftMargin * 1.5;
    auto dialSize = bounds.getWidth() * 0.25;
    auto labelWidth = lowCutSliderLabel.getWidth();
    auto labelHeight = lowCutSliderLabel.getHeight();
    auto bypassButtonSize = bounds.getWidth() * 0.10;
    auto irLoaderButtonWidth = irLoader1.getWidth();
    auto irLoaderButtonHeight = irLoader1.getHeight();
    
    IRGroup.setBounds(leftMargin, y, groupWidth, height);
    EQGroup.setBounds(IRGroup.getRight() + leftMargin, IRGroup.getY(), groupWidth, height);
    distGroup.setBounds(leftMargin, IRGroup.getBottom() + y, groupWidth, height);
    delayGroup.setBounds(EQGroup.getX(), distGroup.getY(), groupWidth, height);
    
//    IR GROUP
    irBypassButton.setBounds(IRGroup.getWidth() * 0.9, IRGroup.getHeight() * 0.05, bypassButtonSize, bypassButtonSize);

    irLoader1.setBounds(IRGroup.getWidth() * 0.1, IRGroup.getHeight() * 0.16, irLoaderButtonWidth , irLoaderButtonHeight);
    irLoader2.setBounds(irLoader1.getX(), irLoader1.getBottom() * 1.1, irLoaderButtonWidth, irLoaderButtonHeight);
    
    lowCutSlider.setBounds(IRGroup.getWidth() * 0.1, IRGroup.getHeight() * 0.65, dialSize, dialSize);
    lowCutSliderLabel.setBounds(lowCutSlider.getX(), lowCutSlider.getY() * 0.85, labelWidth, labelHeight);
    highCutSlider.setBounds(IRGroup.getRight() - dialSize * 0.8, lowCutSlider.getY(), dialSize, dialSize);
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
                irLoader1.setButtonText(loadedIRFile1->getFileName());
                irLoader1.setColour(juce::TextButton::ColourIds::textColourOffId, juce::Colours::white);
            }
            else if (irIndex == 2)
            {
                loadedIRFile2 = std::make_unique<juce::File>(selectedFile);
                audioProcessor.loadIR1(*loadedIRFile2);
                irLoader2.setButtonText(loadedIRFile2->getFileName());
                irLoader2.setColour(juce::TextButton::ColourIds::textColourOffId, juce::Colours::white);
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
            irLoader1.setButtonText(file.getFileName());
            irLoader1.setColour(juce::TextButton::ColourIds::textColourOffId, juce::Colours::white);
        }
    }

    auto ir2Path = audioProcessor.apvts.state.getProperty("IR2FilePath", "").toString();
    if (ir2Path.isNotEmpty())
    {
        juce::File file(ir2Path);
        if (file.existsAsFile())
        {
            loadedIRFile2 = std::make_unique<juce::File>(file);
            irLoader2.setButtonText(file.getFileName());
            irLoader2.setColour(juce::TextButton::ColourIds::textColourOffId, juce::Colours::white);
        }
    }
}
