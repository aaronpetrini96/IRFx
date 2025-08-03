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
    inline juce::String getIR1PanName()     {return "IR1Pan";}
    inline juce::String getIR2PanName()     {return "IR2Pan";}

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
    inline juce::String getDistModeName() {return "DistMode";}

    // === Delay ===
    inline juce::String getDelayBypassName()  { return "DelayBypass"; }
    inline juce::String getDelayMixName() {return "DelayMix";}
    inline juce::String getDelayFeedbackName() {return "DelayFeedback";}
    inline juce::String getDelayTimeName() {return "DelayTime";}
    inline juce::String getDelayModeName() {return "DelayMode";}
    inline juce::String getDelaySyncName() {return "DelaySync";}
    inline juce::String getDelayNoteName() {return "DelayNote";}


    // === In & Out Gain ===
    inline juce::String getInGainName() {return "InGain";}
    inline juce::String getOutGainName() {return "OutGain";}
    
    // === General ===
    inline juce::String getGeneralBypassName()  { return "PluginBypass"; }
    inline juce::String getOutputMonoStereoName() {return "OutputMonoStereo";}



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
            getIR1PanName(),
            getIR2PanName(),
            getEQBypassName(),
            getEQLowGainName(),
            getEQMidGainName(),
            getEQMidFreqName(),
            getEQHighGainName(),
            getDistBypassName(),
            getDistDriveName(),
            getDistMixName(),
            getDistModeName(),
            getDelayBypassName(),
            getDelayMixName(),
            getDelayFeedbackName(),
            getDelayTimeName(),
            getDelayModeName(),
            getDelaySyncName(),
            getDelayNoteName(),
            getInGainName(),
            getOutGainName(),
            getGeneralBypassName(),
            getOutputMonoStereoName(),
        };
    }
}
