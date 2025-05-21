/*
  ==============================================================================

    PluginEffects.h
    Created: 16 May 2025 11:06:40am
    Author:  Marco65701

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

class CMLSPROJECTJUCEEffects
{
public:
    // Audio processing
    juce::AudioSampleBuffer dbuf;
    int bufferSize = 100000;
    int dw[3]{};
    int dr[3]{};

    void setupMixerUI(std::function<void(juce::Component&)> addFn);

	void processDelay(float* left, float* right);
    void processReverb(float* left, float* right, int numSamples);
    void processDistortion(float* sample, float* sampleR);

    int getDelayDS(int SuperCollComponent);
private:


    enum EffectID {
        Reverb,
        Delay,
        Distortion
    };

    struct EffectUIBlock
    {
        juce::Slider* (sliders);  // puntatore ad array di 3 Slider
        juce::Label* (labels);   // puntatore ad array di 3 Label
        juce::String* labelNames;
        int numParams;

        EffectUIBlock(juce::Slider* (sliders), juce::Label* (labels), juce::String* names, int numParams)
            : sliders(sliders),
            labels(labels),
            labelNames(names),
            numParams(numParams)
        {
        }
    };

    // - Reverb ----------------------------------------------------------------------------------------------------------
    juce::Reverb reverbHandler;
    juce::Label reverbMainLabel;
    
    // - Knobs -
    juce::Slider reverbSliders[6];
    juce::Label reverbLabels[6];

    juce::String reverbLabelNames[6] = { "Room Size", "Damping", "Wet level", "Dry level", "Width", "Freeze Mode" };

    EffectUIBlock reverbBlock = { reverbSliders, reverbLabels, reverbLabelNames, 6 };

    // - Distortion ----------------------------------------------------------------------------------------------------------
    juce::Label distortionMainLabel;


    juce::Slider distortionSliders[2];
    juce::Label distortionLabels[2];

    juce::String distortionLabelNames[2] = { "Drive", "Mix" };

    EffectUIBlock distortionBlock = { distortionSliders, distortionLabels, distortionLabelNames, 2 };

	// - Delay ----------------------------------------------------------------------------------------------------------
	juce::Label delayMainLabel;

	juce::Slider delaySliders[3];
	juce::Label delayLabels[3];

	juce::String delayLabelNames[3] = { "Wet Level", "Dry Level", "Delay seconds"};

	EffectUIBlock delayBlock = { delaySliders, delayLabels, delayLabelNames, 3 };

    // - FUNCTIONS AND PROCEDURES ------------------------------------------------------------------------------------------------------------

    void setUpArea(std::function<void(juce::Component&)> addFn, int area, int posX, int posY);
    void retrieveData(int effectID, EffectUIBlock*& block);
    void setMainLabel(std::function<void(juce::Component&)> addFn, int area, int posX, int posY);

};