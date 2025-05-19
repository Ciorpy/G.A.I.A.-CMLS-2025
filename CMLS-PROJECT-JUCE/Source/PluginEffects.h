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
    // Variables related to effects
    float wet;
    float dry;
    int ds = 44100;

    float delayVal = 0.0f;
    float reverbVal = 0.0f;
    float distortionVal = 0.0f;
    float octaverVal = 0.0f;

    std::array<float*, 4> effectsValuesArray = { &distortionVal, &delayVal, &reverbVal, &octaverVal };


    void setupMixerUI(std::function<void(juce::Component&)> addFn);

    void processDelay(float* sampleL, float* sampleR, float delayVal);
    void processReverb(float* left, float* right, int numSamples, int SuperCollComponent);
    void processDistortion(float* sample, float* sampleR, int SuperCollComponent);

private:
    enum EffectID {
        WahWah,
        Reverb,
        Delay,
        Envelope,
        Distortion,
        Octaver
    };

    enum SuperColliderComponent {
        Chords,
        Bass,
        Notes
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
    juce::Reverb reverbHandler;
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


    // - FUNCTIONS AND PROCEDURES ------------------------------------------------------------------------------------------------------------

    void setUpArea(std::function<void(juce::Component&)> addFn, int area, int posX, int posY);
    void retrieveData(int effectID, EffectUIBlock*& block);
    void setMainLabel(std::function<void(juce::Component&)> addFn, int area, int posX, int posY);

};