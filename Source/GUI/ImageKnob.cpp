#include "ImageKnob.h"

ImageKnob::ImageKnob (const juce::String& sliderName, juce::AudioProcessorValueTreeState& apvts, const juce::ParameterID& parameterID, const juce::Image& knobImage, const juce::String& suffix, bool drawFromMiddle)
: attachment(apvts, parameterID.getParamID(), slider)
{
    imageKnobLookAndFeel = std::make_unique<ImageKnobLookAndFeel>(knobImage);

    // Slider
    slider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    slider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    slider.setLookAndFeel(imageKnobLookAndFeel.get());
    slider.getProperties().set("sliderName", sliderName);
    slider.setBounds(0,0, 80, 80);
    // Update value label when the slider changes
    slider.onValueChange = [this, suffix]()
    {
        auto value = std::abs(slider.getValue()) < 0.01 ? 0.0 : std::round(slider.getValue() * 100.0) / 100.0;
        valueLabel.setText(juce::String(value, 0) + suffix, juce::dontSendNotification);
    };
    
//    Draw from middle?
    slider.setRotaryParameters(1.25f*pi, 2.75f*pi, true);
    slider.getProperties().set("drawFromMiddle", drawFromMiddle);
    addAndMakeVisible(slider);


    // Value label
    valueLabel.setJustificationType(juce::Justification::centred);
    valueLabel.setColour(juce::Label::textColourId, juce::Colours::white.darker(0.2f));
    valueLabel.setSize(80, 20);
    addAndMakeVisible(valueLabel);
    valueLabel.setText(juce::String(slider.getValue(), 0) + suffix, juce::dontSendNotification);
    
    slider.onValueChange();
}

ImageKnob::~ImageKnob()
{
    slider.setLookAndFeel(nullptr);
}


void ImageKnob::resized()
{
    valueLabel.setBounds(0, slider.getBottom() * 0.9, valueLabel.getWidth(), valueLabel.getHeight());
}




