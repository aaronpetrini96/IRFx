#pragma once

#include <JuceHeader.h>

// This namespace holds ALL parameter ID strings in one place
namespace ParamNames
{
    // === IR Loader ===
    inline juce::String getIRBypassName()     { return "IRBypass"; }
    inline juce::String getIRLowCutName()     { return "IRLowCut"; }
    inline juce::String getIRHighCutName()    { return "IRHighCut"; }
    inline juce::String getIR1LevelName()    { return "IR1Level"; }
    inline juce::String getIR2LevelName()    { return "IR2Level"; }

    // === EQ ===
    inline juce::String getEQBypassName()     { return "EQBypass"; }
    inline juce::String getEQLowGainName()    { return "EQLowGain"; }
    inline juce::String getEQMidGainName()    { return "EQMidGain"; }
    inline juce::String getEQMidFreqName()    { return "EQMidFreq"; }
    inline juce::String getEQHighGainName()   { return "EQHighGain"; }

    // === Distortion ===
    inline juce::String getDistBypassName()   { return "DistBypass"; }
    inline juce::String getDistDriveName() {return "DistDrive";}
    inline juce::String getDistMixName() {return "DistMix";}

    // === Delay ===
    inline juce::String getDelayBypassName()  { return "DelayBypass"; }

    // === In & Out Gain ===
    inline juce::String getInGainName() {return "InGain";}
    inline juce::String getOutGainName() {return "OutGain";}

    // === If you need to loop over all IDs ===
    // Example: an array of all IDs for simpler iteration
    inline const juce::StringArray getAllIDs()
    {
        return {
            getIRBypassName(),
            getIRLowCutName(),
            getIRHighCutName(),
            getIR1LevelName(),
            getIR2LevelName(),
            getEQBypassName(),
            getEQLowGainName(),
            getEQMidGainName(),
            getEQMidFreqName(),
            getEQHighGainName(),
            getDistBypassName(),
            getDistDriveName(),
            getDistMixName(),
            getDelayBypassName()
        };
    }
}
