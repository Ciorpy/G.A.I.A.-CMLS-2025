/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
CMLSPROJECTJUCEAudioProcessorEditor::CMLSPROJECTJUCEAudioProcessorEditor (CMLSPROJECTJUCEAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    wetSlider.setRange(0.0, 1.0);
    wetSlider.setTextBoxStyle(juce::Slider::TextBoxRight, false, 100, 20);
    wetSlider.addListener(this);
    wetLabel.setText("Wet Level", juce::dontSendNotification);

    addAndMakeVisible(wetSlider);
    addAndMakeVisible(wetLabel);

    drySlider.setRange(0.0, 1.0);
    drySlider.setTextBoxStyle(juce::Slider::TextBoxRight, false, 100, 20);
    drySlider.addListener(this);
    dryLabel.setText("Dry Level", juce::dontSendNotification);

    addAndMakeVisible(drySlider);
    addAndMakeVisible(dryLabel);

    timeSlider.setRange(500, 50000, 100);
    timeSlider.setTextBoxStyle(juce::Slider::TextBoxRight, false, 100, 20);
    timeSlider.addListener(this);
    timeLabel.setText("Time", juce::dontSendNotification);

    addAndMakeVisible(timeSlider);
    addAndMakeVisible(timeLabel);
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize (400, 300);
}

CMLSPROJECTJUCEAudioProcessorEditor::~CMLSPROJECTJUCEAudioProcessorEditor()
{
}

//==============================================================================
void CMLSPROJECTJUCEAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));

    g.setColour (juce::Colours::white);
    g.setFont (juce::FontOptions (15.0f));
    g.drawFittedText ("AnemoSYNTH V1", getLocalBounds(), juce::Justification::centred, 1);
}

void CMLSPROJECTJUCEAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
    wetLabel.setBounds(10, 10, 90, 20);
    wetSlider.setBounds(100, 10, getWidth() - 110, 20);

    dryLabel.setBounds(10, 50, 90, 20);
    drySlider.setBounds(100, 50, getWidth() - 110, 20);

    timeLabel.setBounds(10, 90, 90, 20);
    timeSlider.setBounds(100, 90, getWidth() - 110, 20);
}

void CMLSPROJECTJUCEAudioProcessorEditor::sliderValueChanged(juce::Slider* slider)
{
    if (slider == &wetSlider)
        audioProcessor.set_wet(wetSlider.getValue());
    else if (slider == &drySlider)
        audioProcessor.set_dry(drySlider.getValue());
    else if (slider == &timeSlider)
        audioProcessor.set_ds(timeSlider.getValue());
}
