#pragma once
#include <JuceHeader.h>
#include "ImageKnobLookAndFeel.h"

class ImageKnob : public juce::Component
{
public:
    ImageKnob (const juce::String& sliderName,
               juce::AudioProcessorValueTreeState& apvts,
               const juce::ParameterID& parameterID,
               const juce::Image& knobImage,
               const juce::String& suffix,
               bool drawFromMiddle = false);
    
    ~ImageKnob() override;
    
//    void paint (juce::Graphics&) override;
    void resized() override;
    
    
private:
    juce::Image knobImage;
    juce::Slider slider;
    juce::AudioProcessorValueTreeState::SliderAttachment attachment;
    juce::Label valueLabel;
    
    std::unique_ptr<ImageKnobLookAndFeel> imageKnobLookAndFeel;
    
    const float pi = juce::MathConstants<float>::pi;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ImageKnob)
};
