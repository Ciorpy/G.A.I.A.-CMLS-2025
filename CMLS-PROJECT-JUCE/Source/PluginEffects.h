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
    enum SuperColliderComponent {
        Chords,
        Bass,
        Notes
    };

    // Audio processing
    juce::AudioSampleBuffer dbuf;
    int bufferSize = 100000;
    int dw[3]{};
    int dr[3]{};

    void setupMixerUI(std::function<void(juce::Component&)> addFn);

	void processDelay(float* left, float* right, int SuperCollComponent);
    void processReverb(float* left, float* right, int numSamples, int SuperCollComponent);
    void processDistortion(float* sample, float* sampleR, int SuperCollComponent);

    int getDelayDS(int SuperCollComponent);
private:


    enum EffectID {
        WahWah,
        Reverb,
        Delay,
        Envelope,
        Distortion,
        Octaver
    };

    struct EffectUIBlock
    {
        juce::Slider(*sliders)[3];  // puntatore ad array di 3 Slider
        juce::Label(*labels)[3];   // puntatore ad array di 3 Label
        juce::String* labelNames;
        int numParams;

        EffectUIBlock(juce::Slider(*sliders)[3], juce::Label(*labels)[3], juce::String* names, int numParams)
            : sliders(sliders),
            labels(labels),
            labelNames(names),
            numParams(numParams)
        {
        }
    };

    // - Reverb ----------------------------------------------------------------------------------------------------------
    juce::Reverb reverbHandler[3];
    juce::Label reverbMainLabel;
    
    // - Knobs -
    juce::Slider reverbSliders[6][3];
    juce::Label reverbLabels[6][3];

    juce::String reverbLabelNames[6] = { "Room Size", "Damping", "Wet level", "Dry level", "Width", "Freeze Mode" };

    EffectUIBlock reverbBlock = { reverbSliders, reverbLabels, reverbLabelNames, 6 };

    // - Distortion ----------------------------------------------------------------------------------------------------------
    juce::Label distortionMainLabel;


    juce::Slider distortionSliders[2][3];
    juce::Label distortionLabels[2][3];

    juce::String distortionLabelNames[2] = { "Drive", "Mix" };

    EffectUIBlock distortionBlock = { distortionSliders, distortionLabels, distortionLabelNames, 2 };

	// - Delay ----------------------------------------------------------------------------------------------------------
	juce::Label delayMainLabel;

	juce::Slider delaySliders[3][3];
	juce::Label delayLabels[3][3];

	juce::String delayLabelNames[3] = { "Wet Level", "Dry Level", "Delay seconds"};

	EffectUIBlock delayBlock = { delaySliders, delayLabels, delayLabelNames, 3 };

    // - FUNCTIONS AND PROCEDURES ------------------------------------------------------------------------------------------------------------

    void setUpArea(std::function<void(juce::Component&)> addFn, int area, int posX, int posY);
    void retrieveData(int effectID, EffectUIBlock*& block);
    void setMainLabel(std::function<void(juce::Component&)> addFn, int area, int posX, int posY);

};