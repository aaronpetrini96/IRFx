/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
IRFxAudioProcessorEditor::IRFxAudioProcessorEditor (IRFxAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p), presetManager(p, presetBox)
{
    startTimerHz(30);
//    IN-OUT GAIN
    addAndMakeVisible(inputGainSlider);
    addAndMakeVisible(outputGainSlider);
    addAndMakeVisible(generalBypassButton);
    addAndMakeVisible(outputMonoStereoBox);

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
    
    
//  PRESET BUTTONS
    setPresetButtonStyle(savePresetButton);
    addAndMakeVisible(savePresetButton);
    savePresetButton.onClick = [this] {presetManager.savePreset();};
    
//  PRESET DROPDOWN MENU
    presetBox.setTextWhenNothingSelected("Select Preset");
    presetBox.setColour(juce::ComboBox::ColourIds::backgroundColourId, juce::Colour(100, 100, 110).darker(0.5f));
    presetBox.setColour(juce::ComboBox::ColourIds::outlineColourId, juce::Colours::transparentBlack);
    presetBox.setLookAndFeel(ComboBoxLookAndFeel::get());
    presetBox.onChange = [this]() {presetManager.presetSelected();};
    presetManager.refreshPresetList();
    addAndMakeVisible(presetBox);
    
    
//  delay BUTTONS
    delaySyncButton.setClickingTogglesState(true);
    setSatButtonColour(delaySyncButton, delaySyncButton.getToggleState());
    delaySyncButton.setColour(juce::ComboBox::outlineColourId, juce::Colours::transparentBlack);
    delaySyncButton.setColour(juce::TextButton::ColourIds::textColourOnId, juce::Colours::white);
    delaySyncButton.setColour(juce::TextButton::ColourIds::textColourOffId, juce::Colours::white.withAlpha(0.3f));
    
    delaySyncButton.onClick = [this]()
    {
        isSyncOn = delaySyncButton.getToggleState();
        audioProcessor.delaySyncParam->beginChangeGesture();
        audioProcessor.delaySyncParam->setValueNotifyingHost(isSyncOn);
        audioProcessor.delaySyncParam->endChangeGesture();
        updateDelaySyncState();
    };
    
    
    
//  DELAY BOXES
//    delayMonoStereoBox.addItem("Centre", 1);
//    delayMonoStereoBox.addItem("Wide", 2);
//    delayMonoStereoBox.setSelectedId(1);
//    delayMonoStereoBox.setColour(juce::ComboBox::ColourIds::backgroundColourId, darkPink);
//    delayMonoStereoBox.setColour(juce::ComboBox::ColourIds::outlineColourId, juce::Colours::transparentBlack);
//    delayMonoStereoBox.setLookAndFeel(ComboBoxLookAndFeel::get());
//    delayMonoStereoAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(audioProcessor.apvts, ParamNames::getDelayMonoStereoName(), delayMonoStereoBox);
    
    
    delayModeBox.addItem("Digital", 1);
    delayModeBox.addItem("Tape", 2);
    delayModeBox.setSelectedId(1);
    delayModeBox.setColour(juce::ComboBox::ColourIds::backgroundColourId, darkPink);
    delayModeBox.setColour(juce::ComboBox::ColourIds::outlineColourId, juce::Colours::transparentBlack);
    delayModeBox.setLookAndFeel(ComboBoxLookAndFeel::get());
    delayModeAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(audioProcessor.apvts, ParamNames::getDelayModeName(), delayModeBox);
    
    
//    OUTPUT BOXES
    outputMonoStereoBox.addItem("Mono", 1);
    outputMonoStereoBox.addItem("Stereo", 2);
    outputMonoStereoBox.setSelectedId(1);
    outputMonoStereoBox.setColour(juce::ComboBox::ColourIds::backgroundColourId, juce::Colour(100, 100, 110).darker(0.5f));
    outputMonoStereoBox.setColour(juce::ComboBox::ColourIds::outlineColourId, juce::Colours::transparentBlack);
    outputMonoStereoBox.setLookAndFeel(ComboBoxLookAndFeel::get());
    outputMonoStereoBoxAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(audioProcessor.apvts, ParamNames::getOutputMonoStereoName(), outputMonoStereoBox);
    
    
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
        button->setSize(125, 40);
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
            audioProcessor.irLoader1->loadImpulseResponse(juce::AudioBuffer<float>(), audioProcessor.getSampleRate(), juce::dsp::Convolution::Stereo::yes, juce::dsp::Convolution::Trim::no, juce::dsp::Convolution::Normalise::no);
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
            audioProcessor.irLoader2->loadImpulseResponse(juce::AudioBuffer<float>(), audioProcessor.getSampleRate(), juce::dsp::Convolution::Stereo::yes, juce::dsp::Convolution::Trim::no, juce::dsp::Convolution::Normalise::no);
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
    

//   SAT MODE BUTTONS
    sat1Button.onClick = [this] () {setSaturationType(0);};
    sat2Button.onClick = [this] () {setSaturationType(1);};
    sat3Button.onClick = [this] () {setSaturationType(2);};
    for (auto button : satModeButtons)
    {
        button -> setClickingTogglesState(true);
        button -> setRadioGroupId(123);
        button -> setSize(60, 30);
        button -> setColour(juce::ComboBox::ColourIds::outlineColourId, juce::Colours::transparentBlack);
        setSatButtonColour(*button, button->getToggleState());
        distGroup.addAndMakeVisible(*button);
    }

    
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
    IRGroup.addAndMakeVisible(ir1PanSlider);
    IRGroup.addAndMakeVisible(ir2PanSlider);
    
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
    distGroup.addAndMakeVisible(satModeLabel);
    
    delayGroup.addAndMakeVisible(delayBypassButton);
    delayGroup.addAndMakeVisible(delayMixKnob);
    delayGroup.addAndMakeVisible(delayMixLabel);
    delayGroup.addAndMakeVisible(delayFeedbackKnob);
    delayGroup.addAndMakeVisible(delayFeedbackLabel);
    delayGroup.addAndMakeVisible(delayTimeKnob);
    delayGroup.addAndMakeVisible(delayTimeLabel);
    delayGroup.addAndMakeVisible(delayNoteKnob);
    delayGroup.addAndMakeVisible(delaySettingsLabel);
    delayGroup.addAndMakeVisible(delaySyncButton);
    delayGroup.addAndMakeVisible(delayModeBox);
   
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    ir1LevelSlider.setVisible(false);
    ir2LevelSlider.setVisible(false);
    delayNoteKnob.setVisible(false);
    restoreLoadedIRFiles();
    updateDelaySyncState();
    if (!audioProcessor.outputIsStereo)
    {
        ir1PanSlider.setVisible(false);
        ir2PanSlider.setVisible(false);
    }
    
    setSize (600, 650);
}

IRFxAudioProcessorEditor::~IRFxAudioProcessorEditor()
{
}

//==============================================================================
void IRFxAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (juce::Colours::black);
    g.setColour(juce::Colours::grey.withAlpha(0.1f));
    g.fillRect(0, 615, 600, 35);

    
    clipLight(g);
    
    g.setColour(juce::Colours::white.withAlpha(0.9f));
    auto fontOptions = juce::FontOptions("Helvetica", getHeight() * 0.025, juce::Font::FontStyleFlags::bold);
    g.setFont(juce::Font(fontOptions));
    g.drawFittedText("IRFx v1.0", getWidth() * 0.1, getHeight() * 0.957, 200, 20, juce::Justification::centredLeft, 1);
    
    auto size = getWidth() * 0.05;
    if (screwImage.isValid())
    {
        g.drawImage(screwImage, IRGroup.getX() - size * 0.25, getHeight() * 0.955, size, size, 0, 0, getWidth() * 0.1, getWidth() * 0.1);
        g.drawImage(screwImage, delayGroup.getRight() - size * 0.5, getHeight() * 0.955, size, size, 0, 0, getWidth() * 0.1, getWidth() * 0.1);
    }
}

void IRFxAudioProcessorEditor::resized()
{
    auto totalBounds = getLocalBounds();
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
    auto satModeButtonWidth = boundsWidth * 0.1;

    IRGroup.setBounds(leftMargin, y, groupWidth, height);
    EQGroup.setBounds(IRGroup.getRight() + leftMargin, IRGroup.getY(), groupWidth, height);
    distGroup.setBounds(leftMargin, IRGroup.getBottom() + y, groupWidth, height);
    delayGroup.setBounds(EQGroup.getX(), distGroup.getY(), groupWidth, height);
    
    
//   PRESET BOX
    presetBox.setBounds(totalBounds.getWidth() * 0.285, totalBounds.getHeight() * 0.955, boundsWidth * 0.25 , boundsWidth * 0.04);
    savePresetButton.setBounds(presetBox.getRight() * 1.05, presetBox.getY(), presetBox.getWidth() * 0.5, presetBox.getHeight());
    
    outputMonoStereoBox.setBounds(savePresetButton.getRight() * 1.04, savePresetButton.getY(), savePresetButton.getWidth(), savePresetButton.getHeight());
    
//    IR GROUP
    irBypassButton.setBounds(IRGroup.getWidth() * 0.9, IRGroup.getHeight() * 0.05, bypassButtonSize, bypassButtonSize);

    irLoader1Button.setBounds(IRGroup.getWidth() * 0.065, IRGroup.getHeight() * 0.16, irLoaderButtonWidth , irLoaderButtonHeight);
    unloadIR1Button.setBounds(irLoader1Button.getRight() * 1.05, irLoader1Button.getY() + (irLoaderButtonHeight - unloadIRButtonSize) * 0.5, unloadIRButtonSize, unloadIRButtonSize);
    muteIR1Button.setBounds(unloadIR1Button.getRight() * 1.05, irLoader1Button.getY() + (irLoaderButtonHeight - muteIRButtonSize) * 0.5, muteIRButtonSize, muteIRButtonSize);
    ir1LevelSlider.setBounds(irLoader1Button.getX(), IRGroup.getHeight() * 0.055, irLevelSliderWidth, irLevelSliderHeight);
    
    irLoader2Button.setBounds(irLoader1Button.getX(), irLoader1Button.getBottom() * 1.1, irLoaderButtonWidth, irLoaderButtonHeight);
    unloadIR2Button.setBounds(unloadIR1Button.getX(), irLoader2Button.getY() + (irLoaderButtonHeight - unloadIRButtonSize) * 0.5, unloadIRButtonSize, unloadIRButtonSize);
    muteIR2Button.setBounds(unloadIR2Button.getRight() * 1.05, irLoader2Button.getY() + (irLoaderButtonHeight - muteIRButtonSize) * 0.5, muteIRButtonSize, muteIRButtonSize);
    ir2LevelSlider.setBounds(irLoader2Button.getX(), irLoader2Button.getY() * 1.45, irLevelSliderWidth, irLevelSliderHeight);
    
    lowCutSlider.setBounds(IRGroup.getWidth() * 0.11, IRGroup.getHeight() * 0.65, dialSize * 0.75, dialSize * 0.75);
    lowCutSliderLabel.setBounds(lowCutSlider.getX(), lowCutSlider.getY() * 0.85, labelWidth, labelHeight);
    highCutSlider.setBounds(IRGroup.getRight() - dialSize * 0.85, lowCutSlider.getY(), dialSize * 0.75, dialSize * 0.75);
    highCutSliderLabel.setBounds(highCutSlider.getX(), highCutSlider.getY() * 0.85, labelWidth, labelHeight);
    
    auto irPanDialSize = dialSize * 0.35;
    ir1PanSlider.setBounds(muteIR1Button.getRight() * 1.04, muteIR1Button.getY(), irPanDialSize, irPanDialSize);
    ir2PanSlider.setBounds(ir1PanSlider.getX(), muteIR2Button.getY(), irPanDialSize, irPanDialSize);
    
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
    
    sat1Button.setBounds(distGroup.getWidth() * 0.18, distGroup.getHeight() * 0.8, satModeButtonWidth, satModeButtonWidth * 0.4);
    sat2Button.setBounds(sat1Button.getRight(), sat1Button.getY(), satModeButtonWidth, satModeButtonWidth * 0.4);
    sat3Button.setBounds(sat2Button.getRight(), sat2Button.getY(), satModeButtonWidth, satModeButtonWidth * 0.4);
    satModeLabel.setBounds(sat2Button.getX() * 0.92, sat2Button.getY() * 1.11, labelWidth, labelHeight);
    
//    DELAY GROUP
    delayBypassButton.setBounds(delayGroup.getWidth() * 0.9, delayGroup.getHeight() * 0.05, bypassButtonSize, bypassButtonSize);
    delayTimeKnob.setBounds(delayGroup.getWidth() * 0.1, delayGroup.getHeight() * 0.2, dialSize, dialSize);
    delayNoteKnob.setBounds(delayGroup.getWidth() * 0.1, delayGroup.getHeight() * 0.2, dialSize, dialSize);
    delayTimeLabel.setBounds(delayTimeKnob.getX(), delayTimeKnob.getY() * 0.5, labelWidth, labelHeight);
    delayFeedbackKnob.setBounds(delayGroup.getWidth() * 0.6, delayTimeKnob.getY(), dialSize, dialSize);
    delayFeedbackLabel.setBounds(delayFeedbackKnob.getX(), delayTimeLabel.getY(), labelWidth, labelHeight);
    delayMixKnob.setBounds(delayTimeKnob.getX(), delayGroup.getHeight() * 0.65, dialSize, dialSize);
    delayMixLabel.setBounds(delayMixKnob.getX(), delayMixKnob.getY() * 0.85, labelWidth, labelHeight);
    
    delaySettingsLabel.setBounds(delayFeedbackKnob.getX(), delayMixLabel.getY(), labelWidth, labelHeight);
    
    delaySyncButton.setBounds(delayFeedbackKnob.getX(), delayFeedbackKnob.getBottom() * 0.95, savePresetButton.getWidth(), savePresetButton.getHeight());
    delayModeBox.setBounds(delaySyncButton.getX(), delaySyncButton.getBottom() * 1.05, savePresetButton.getWidth(), savePresetButton.getHeight());
 
//    IN OUT GAIN
    inputGainSlider.setBounds(IRGroup.getX(), distGroup.getBottom() * 0.98, inputGainSlider.getWidth(), inputGainSlider.getHeight());
    outputGainSlider.setBounds(delayGroup.getX() * 1.09, inputGainSlider.getY(), outputGainSlider.getWidth(), outputGainSlider.getHeight());
    
    generalBypassButton.setBounds(outputMonoStereoBox.getRight() * 1.059, outputMonoStereoBox.getY() * 1.0025, bypassButtonSize, bypassButtonSize);

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
            audioProcessor.loadIR1(*loadedIRFile1);
            irLoader1Button.setButtonText(loadedIRFile1->getFileName());
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
            audioProcessor.loadIR2(*loadedIRFile2);
            irLoader2Button.setButtonText(loadedIRFile2->getFileName());
            irLoader2Button.setColour(juce::TextButton::ColourIds::textColourOffId, juce::Colours::white);
            ir2LevelSlider.setVisible(true);
        }
    }
}



void IRFxAudioProcessorEditor::timerCallback()
{
//    SAT TYPE
    auto currentSatMode = audioProcessor.saturationModeParam->getIndex();
    switch (currentSatMode)
    {
        case 0:
            setSatButtonColour(sat1Button, true);
            break;
        case 1:
            setSatButtonColour(sat2Button, true);
            break;
        case 2:
            setSatButtonColour(sat3Button, true);
            break;
    }
    sat1Button.setToggleState(currentSatMode == 0, juce::dontSendNotification);
    sat2Button.setToggleState(currentSatMode == 1, juce::dontSendNotification);
    sat3Button.setToggleState(currentSatMode == 2, juce::dontSendNotification);
    

    if (audioProcessor.outputIsStereo)
    {
        ir1PanSlider.setVisible(true);
        ir2PanSlider.setVisible(true);

    }
    else
    {
        ir1PanSlider.setVisible(false);
        ir2PanSlider.setVisible(false);

    }
    
//    if (!audioProcessor.delayIsMono) {
//        outputMonoStereoBox.setSelectedItemIndex(1);
//    }


// === Clipping light logic ===
    
    bool inClipping = audioProcessor.clipFlagIn.exchange(false);
    bool outClipping = audioProcessor.clipFlagOut.exchange(false);
    if (inClipping)
        inputGainSlider.setClippingStatus(true);
    if (outClipping)
        outputGainSlider.setClippingStatus(true);
    
    if (inClipping == true || outClipping == true)
    {
        isClippingLightOn = true;
        clipLightHoldCounter = 15; // light stays on for 15 ticks
        clipPopScale = 1.3f;       // pop to 1.3x size when clipping
    }
    
    if (clipLightHoldCounter > 0)
    {
        --clipLightHoldCounter;
        
        // Gradually shrink pop effect back to normal
        clipPopScale -= 0.02f;
        if (clipPopScale < 1.0f)
            clipPopScale = 1.0f;
    }
    else
    {
        inputGainSlider.setClippingStatus(false);
        outputGainSlider.setClippingStatus(false);
        isClippingLightOn = false;
        clipPopScale = 1.0f;
    }
    
    repaint(); // triggers paint() to draw updated light
}

void IRFxAudioProcessorEditor::updateDelaySyncState()
{
    isSyncOn = audioProcessor.delaySyncParam->get();
    delaySyncButton.setToggleState(isSyncOn, juce::dontSendNotification);
    setSatButtonColour(delaySyncButton, isSyncOn);
    delayTimeKnob.setVisible(!isSyncOn);
    delayNoteKnob.setVisible(isSyncOn);
}

void IRFxAudioProcessorEditor::clipLight(juce::Graphics& g)
{
    juce::Colour activeColor = juce::Colours::red.withAlpha(0.9f);
    juce::Colour offColor = juce::Colours::green.withAlpha(0.9f);
    juce::Colour currentColor = isClippingLightOn ? activeColor : offColor;
   
    g.setColour(currentColor);
    auto fontOptions = juce::FontOptions(20.0f * clipPopScale, juce::Font::bold);
    g.setFont(juce::Font(fontOptions));
    g.drawFittedText("CLIP", getWidth() * 0.334, getHeight() * 0.909, 200, 20, juce::Justification::centred, 1);
}


void IRFxAudioProcessorEditor::setSaturationType(int type)
{
    audioProcessor.saturationModeParam->beginChangeGesture();
    audioProcessor.saturationModeParam->setValueNotifyingHost(audioProcessor.saturationModeParam->convertTo0to1(type));
    audioProcessor.saturationModeParam->endChangeGesture();
    
    setSatButtonColour(sat1Button, sat1Button.getToggleState());
    setSatButtonColour(sat2Button, sat2Button.getToggleState());
    setSatButtonColour(sat3Button, sat3Button.getToggleState());
}

void IRFxAudioProcessorEditor::setSatButtonColour (juce::TextButton& b, bool isOn)
{
    juce::Colour darkPink = juce::Colour::fromRGB(200, 30, 100).withAlpha(0.8f); // Dark Trash pink

    if (isOn)
    {
        b.setColour(juce::TextButton::ColourIds::buttonOnColourId, darkPink.brighter());
    }
    else
    {
        b.setColour(juce::TextButton::ColourIds::buttonColourId, darkPink.withAlpha(0.5f));
    }
}

void IRFxAudioProcessorEditor::setPresetButtonStyle(juce::TextButton& button)
{
    button.setClickingTogglesState(true);
    button.setColour(juce::TextButton::ColourIds::buttonColourId, juce::Colour(100, 100, 110).darker(0.5f));
    button.setColour(juce::TextButton::ColourIds::buttonOnColourId, juce::Colour(100, 100, 110).darker(0.5f));
    button.setColour(juce::ComboBox::outlineColourId, juce::Colours::transparentBlack);
    button.setColour(juce::TextButton::ColourIds::textColourOnId, juce::Colours::white);
    button.setColour(juce::TextButton::ColourIds::textColourOffId, juce::Colours::white);
//    button.setSize(25, 25);
    button.setSize(25, presetBox.getHeight());
}
