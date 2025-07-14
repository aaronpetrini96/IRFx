#include "PresetManager.h"

PresetManager::PresetManager(IRFxAudioProcessor& processor, juce::ComboBox& presetBox) : audioProcessor(processor),
presetBox(presetBox)
{
    
}

void PresetManager::savePreset()
{
    auto presetFolder = getPresetFolder();
    if (!presetFolder.exists())
        presetFolder.createDirectory();
    
    fileChooser = std::make_unique<juce::FileChooser> ("Save Preset", presetFolder, "*.xml");
    
    auto fileChooserFlags = juce::FileBrowserComponent::saveMode;
    
    fileChooser->launchAsync (fileChooserFlags, [this] (const juce::FileChooser& fileChooser)
    {
        juce::File selectedFile = fileChooser.getResult();
        
        if (selectedFile.existsAsFile() || selectedFile.create())
        {
            // Save the preset to the selected file
            audioProcessor.savePreset(selectedFile);
            DBG("Preset saved to: " + selectedFile.getFullPathName());
            
            // Optionally, refresh the preset list in the UI or do other actions
            refreshPresetList();
        }
        else
        {
            DBG("Error: Failed to save preset. File creation failed.");
        };
        
    });
}

void PresetManager::loadPreset()
{
    juce::FileChooser chooser ("Load Preset", juce::File::getSpecialLocation (juce::File::userDocumentsDirectory), "*.xml");

    auto file = chooser.getResult();
    audioProcessor.loadPreset (file);  // Calls your processor’s loadPreset
}

void PresetManager::presetSelected()
{
    auto selectedId = presetBox.getSelectedId();
    if (selectedId > 0)
    {
        auto presetName = presetBox.getItemText(selectedId - 1);
        auto presetFile = getPresetFolder().getChildFile(presetName + ".xml");
        audioProcessor.loadPreset(presetFile);
    }
}

void PresetManager::refreshPresetList()
{
    presetBox.clear();
    auto presetFolder = getPresetFolder();
    presetFolder.createDirectory();
    
    auto files = presetFolder.findChildFiles(juce::File::TypesOfFileToFind::findFiles, false);
    
    int index = 1;
    for (auto& file : files)
    {
        presetBox.addItem(file.getFileNameWithoutExtension(), index);
        ++index;
    }
}

juce::File PresetManager::getPresetFolder()
{
    return juce::File::getSpecialLocation(juce::File::userDocumentsDirectory).getChildFile("IRFx/Presets");
}


