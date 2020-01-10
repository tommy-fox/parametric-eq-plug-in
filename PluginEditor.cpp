// This file contains the framework code for a JUCE plugin GUI
// as well as slider contols for the three band parametric EQ.


#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
ParametricEqAudioProcessorEditor::ParametricEqAudioProcessorEditor (ParametricEqAudioProcessor& p)
    : AudioProcessorEditor (&p), processor (p)
{
    // Size of plugin window
    setSize (600, 250);
    
    auto& params = processor.getParameters();
    
    // Low frequency slider control set up
    AudioParameterFloat* loGainParameter = (AudioParameterFloat*) params.getUnchecked(0);
    mLoGainControlSlider.setBounds(0, 0, 200, 200);
    mLoGainControlSlider.setNumDecimalPlacesToDisplay(0);
    mLoGainControlSlider.setSliderStyle(Slider::SliderStyle::LinearVertical);
    mLoGainControlSlider.setDoubleClickReturnValue(true, 1.0);
    mLoGainControlSlider.setTextValueSuffix (" dB");
    mLoGainControlSlider.setTextBoxStyle(Slider::TextBoxBelow, false, 100, 30);
    mLoGainControlSlider.setTextValueSuffix ("\n200Hz");
    mLoGainControlSlider.setRange(loGainParameter->range.start, loGainParameter->range.end, .01);
    mLoGainControlSlider.setValue(*loGainParameter);
    mLoGainControlSlider.onValueChange = [this, loGainParameter]{*loGainParameter = mLoGainControlSlider.getValue();};
    mLoGainControlSlider.onDragStart = [loGainParameter]{loGainParameter->beginChangeGesture();};
    mLoGainControlSlider.onDragEnd = [loGainParameter]{loGainParameter->endChangeGesture();};
    
    // Mid frequency slider control set up
    AudioParameterFloat* midGainParameter = (AudioParameterFloat*) params.getUnchecked(1);
    mMidGainControlSlider.setBounds(200, 0, 200, 200);
    mMidGainControlSlider.setSliderStyle(Slider::SliderStyle::LinearVertical);
    mMidGainControlSlider.setDoubleClickReturnValue(true, 1.0);
    mMidGainControlSlider.setTextValueSuffix (" dB");
    mMidGainControlSlider.setTextBoxStyle(Slider::TextBoxBelow, false, 100, 30);
    mMidGainControlSlider.setTextValueSuffix ("\n2kHz");
    mMidGainControlSlider.setRange(midGainParameter->range.start, midGainParameter->range.end, .01);
    mMidGainControlSlider.setValue(*midGainParameter);
    mMidGainControlSlider.onValueChange = [this, midGainParameter]{*midGainParameter = mMidGainControlSlider.getValue();};
    mMidGainControlSlider.onDragStart = [midGainParameter]{midGainParameter->beginChangeGesture();};
    mMidGainControlSlider.onDragEnd = [midGainParameter]{midGainParameter->endChangeGesture();};
    
    // High frequency slider control set up
    AudioParameterFloat* hiGainParameter = (AudioParameterFloat*) params.getUnchecked(2);
    mHiGainControlSlider.setBounds(400, 0, 200, 200);
    mHiGainControlSlider.setSliderStyle(Slider::SliderStyle::LinearVertical);
    mHiGainControlSlider.setDoubleClickReturnValue(true, 1.0);
    mHiGainControlSlider.setTextValueSuffix (" dB");
    mHiGainControlSlider.setTextBoxStyle(Slider::TextBoxBelow, false, 100, 30);
    mHiGainControlSlider.setTextValueSuffix ("\n6kHz");
    mHiGainControlSlider.setRange(hiGainParameter->range.start, hiGainParameter->range.end, .01);
    mHiGainControlSlider.setValue(*hiGainParameter);
    mHiGainControlSlider.onValueChange = [this, hiGainParameter]{*hiGainParameter = mHiGainControlSlider.getValue();};
    mHiGainControlSlider.onDragStart = [hiGainParameter]{hiGainParameter->beginChangeGesture();};
    mHiGainControlSlider.onDragEnd = [hiGainParameter]{hiGainParameter->endChangeGesture();};
    
    
    addAndMakeVisible(mLoGainControlSlider);
    addAndMakeVisible(mMidGainControlSlider);
    addAndMakeVisible(mHiGainControlSlider);
}

ParametricEqAudioProcessorEditor::~ParametricEqAudioProcessorEditor(){}

//==============================================================================
void ParametricEqAudioProcessorEditor::paint (Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (ResizableWindow::backgroundColourId));

    g.setColour (Colours::white);
    g.setFont (15.0f);
    g.drawFittedText ("EQ by Tom", getLocalBounds(), Justification::centredBottom, 1);
}

void ParametricEqAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
}

