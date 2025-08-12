/*
  ==============================================================================

    PresetManager.h
    Created: 13 Jul 2025 11:42:50pm
    Author:  Aaron Petrini

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "../PluginProcessor.h"

class PresetManager
{
public:
    PresetManager(IRFxAudioProcessor& processor, juce::ComboBox& presetBox);
    
    void savePreset();
    void loadPreset();
    void presetSelected();
    void refreshPresetList();
    int getItemIdForText(const juce::ComboBox&, const juce::String&);
    
    
private:
    juce::File getPresetFolder();
    IRFxAudioProcessor& audioProcessor;
    juce::ComboBox& presetBox;
    std::unique_ptr<juce::FileChooser> fileChooser;
};
