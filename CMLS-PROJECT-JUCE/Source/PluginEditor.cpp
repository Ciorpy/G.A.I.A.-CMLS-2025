/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "PluginEffects.h"

//==============================================================================
CMLSPROJECTJUCEAudioProcessorEditor::CMLSPROJECTJUCEAudioProcessorEditor (CMLSPROJECTJUCEAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    audioProcessor.setupMixerUI([this](juce::Component& comp) { addAndMakeVisible(comp); });
    setSize (1800, 900);
}

CMLSPROJECTJUCEAudioProcessorEditor::~CMLSPROJECTJUCEAudioProcessorEditor()
{

}

//==============================================================================
void CMLSPROJECTJUCEAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));
    
    //ROW 1
    g.setColour(juce::Colours::red);
    g.fillRect(0, 0, 600, 300);

    g.setColour(juce::Colours::yellow);
    g.fillRect(0, 300, 600, 300);

    g.setColour(juce::Colours::purple);
    g.fillRect(0, 600, 600, 300);

    //ROW 2
    g.setColour(juce::Colours::green);
    g.fillRect(600, 0, 600, 300);

    g.setColour(juce::Colours::cyan);
    g.fillRect(600, 300, 600, 300);

    g.setColour(juce::Colours::darkblue);
    g.fillRect(600, 600, 600, 300);

    //ROW 3
    g.setColour(juce::Colours::blue);
    g.fillRect(1200, 0, 600, 300);

    g.setColour(juce::Colours::magenta);
    g.fillRect(1200, 300, 600, 300);

    g.setColour(juce::Colours::pink);
    g.fillRect(1200, 600, 600, 300);

    g.setColour (juce::Colours::white);
    g.setFont (juce::FontOptions (40.0f));
    g.drawFittedText ("G.A.I.A.", getLocalBounds(), juce::Justification::centredBottom, 1);
}

void CMLSPROJECTJUCEAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
}
