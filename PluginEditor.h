// This is the header definition for the plug-in GUI

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "PluginProcessor.h"

//==============================================================================
/**
*/

class ParametricEqAudioProcessorEditor  : public AudioProcessorEditor
{
public:
    ParametricEqAudioProcessorEditor (ParametricEqAudioProcessor&);
    ~ParametricEqAudioProcessorEditor();

    //==============================================================================
    void paint (Graphics&) override;
    void resized() override;

private:
    ParametricEqAudioProcessor& processor;
    
    Slider mHiGainControlSlider;
    Slider mMidGainControlSlider;
    Slider mLoGainControlSlider;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ParametricEqAudioProcessorEditor)
};
