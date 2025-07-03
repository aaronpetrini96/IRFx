
#include "ImageKnobLookAndFeel.h"

ImageKnobLookAndFeel::ImageKnobLookAndFeel() {}

ImageKnobLookAndFeel::ImageKnobLookAndFeel(const juce::Image& img)
{
    knobImage = img;
}
void ImageKnobLookAndFeel::drawRotarySlider(juce::Graphics &g, int x, int y, int width, int height, float sliderPosProportional, float rotaryStartAngle, float rotaryEndAngle, juce::Slider &slider)
{
    const auto bounds = juce::Rectangle<float>(x, y, width, height);

    if (!knobImage.isValid())
    {
        g.setColour(juce::Colours::red);
        g.drawFittedText("No Image", bounds.toNearestInt(), juce::Justification::centred, 1);
        return;
    }

    // === Common values ===
    const float usableRadius = juce::jmin(width, height) * 0.5f;
    const juce::Point<float> center(x + width * 0.5f, y + height * 0.5f);

    // === Slider value ===
    const double sliderRange = slider.getMaximum() - slider.getMinimum();
    const double rawValue = (slider.getValue() - slider.getMinimum()) / sliderRange;
    const double normalizedValue = juce::jmax(rawValue, 0.0001);

    const int frames = knobImage.getHeight() / knobImage.getWidth();
    const int frameId = (int)std::ceil(rawValue * ((double)frames - 1.0));

    // === Arc angles ===
    const float startAngle = juce::MathConstants<float>::pi * 1.25f;
    const float endAngle   = juce::MathConstants<float>::pi * 2.75f;
    const float valueAngle = startAngle + normalizedValue * (endAngle - startAngle);

    // === Arc appearance ===
//        const float thickness = juce::jlimit(1.5f, 6.0f, usableRadius * 0.15f);
    const float thickness = juce::jlimit(1.5f, 5.0f, usableRadius * 0.12f);
    const float arcRadius = usableRadius - thickness * 0.5f;

//    juce::Colour backgroundFillColor = juce::Colours::black.withAlpha(0.35f);
    juce::Colour backgroundFillColor = juce::Colours::darkgrey.withAlpha(0.9f);
    juce::Colour fillColor = juce::Colour::fromRGB(200, 30, 100).withAlpha(0.8f); // Dark Trash pink

    // === Draw background arc ===
    juce::Path backgroundArc;
    backgroundArc.addCentredArc(center.x, center.y,
                                 arcRadius, arcRadius,
                                 0.0f, startAngle, endAngle, true);

    g.setColour(backgroundFillColor);
    g.strokePath(backgroundArc, juce::PathStrokeType(thickness,
                                                     juce::PathStrokeType::curved,
                                                     juce::PathStrokeType::rounded));

    // === Draw value arc ===
    juce::Path valueArc;
    valueArc.addCentredArc(center.x, center.y,
                           arcRadius, arcRadius,
                           0.0f, startAngle, valueAngle, true);

    g.setColour(fillColor);
    g.strokePath(valueArc, juce::PathStrokeType(thickness,
                                                juce::PathStrokeType::curved,
                                                juce::PathStrokeType::rounded));
    

    // === Draw knob image ===
    const int imageSize = knobImage.getWidth();
    const float imageScale = (usableRadius * 2.0f) / (float)imageSize;
    
    g.setOpacity(0.85f);
    g.drawImageTransformed(
        knobImage.getClippedImage(juce::Rectangle<int>(0, frameId * imageSize, imageSize, imageSize)),
        juce::AffineTransform::scale(imageScale)
                              .translated(center.x - usableRadius, center.y + usableRadius * 0.14f - usableRadius)
    );
    g.setOpacity(1.f);
    
    
    
    
}

