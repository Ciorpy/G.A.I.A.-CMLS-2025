/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

//==============================================================================
/**
*/
class CMLSPROJECTJUCEAudioProcessorEditor  : public juce::AudioProcessorEditor, private juce::Slider::Listener
{
public:
    CMLSPROJECTJUCEAudioProcessorEditor (CMLSPROJECTJUCEAudioProcessor&);
    ~CMLSPROJECTJUCEAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    CMLSPROJECTJUCEAudioProcessor& audioProcessor;

    juce::Slider wetSlider;
    juce::Label wetLabel;
    juce::Slider drySlider;
    juce::Label dryLabel;
    juce::Slider timeSlider;
    juce::Label timeLabel;

    void sliderValueChanged(juce::Slider* slider) override;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (CMLSPROJECTJUCEAudioProcessorEditor)
};
