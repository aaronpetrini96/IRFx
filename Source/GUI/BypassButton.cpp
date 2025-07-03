/*
  ==============================================================================

    ImgButton.cpp
    Created: 30 Apr 2025 3:18:17am
    Author:  Aaron Petrini

  ==============================================================================
*/

#include "BypassButton.h"
#include <JuceHeader.h>



//==============================================================================
BypassButton::BypassButton(const juce::String& text, juce::AudioProcessorValueTreeState& apvts, const juce::ParameterID parameterID)
: attachment(apvts, parameterID.getParamID(), button)
{
    
    auto bypassIcon = juce::ImageCache::getFromMemory(BinaryData::Bypass_png, BinaryData::Bypass_pngSize);
    button.setClickingTogglesState(true);
    button.setBounds(0, 0, 20, 20);
    button.setImages(false, true, true,
                           bypassIcon, 1.f, juce::Colours::white.withAlpha(0.7f),
                           bypassIcon, 1.f, juce::Colours::white.withAlpha(0.7f),
                           bypassIcon, 1.f, juce::Colours::red,
                           0.f);
    button.onClick = [this]
    {
        
    };
    addAndMakeVisible(button);
    setSize(25, 25);
}

BypassButton::~BypassButton()
{
}

void BypassButton::paint (juce::Graphics& g)
{
}

void BypassButton::resized()
{
}
