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
    void processReverb(float* left, float* right, int numSamples);
    void processDistortion(float* sample, float* sampleR);
    void processOctaver(float* sample, float* sampleR, int octaverVal);

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
        juce::Slider** sliders;
        juce::Label** labels;
        juce::String* labelNames;
        int numParams;
    };

    juce::Slider distortionKnob, delayKnob, reverbKnob, octaverKnob;
    std::array<juce::Slider*, 4> sliderArray = {&distortionKnob, &delayKnob, &reverbKnob, &octaverKnob};

    juce::Label distortionLabel, delayLabel, reverbLabel, octaverLabel;
    std::array<juce::Label*, 4> labelArray = { &distortionLabel, &delayLabel, &reverbLabel, &octaverLabel };
    std::array<juce::String, 4> labelNameArray = { "Distortion", "Delay", "Reverb", "Octaver"};

    // - Reverb ----------------------------------------------------------------------------------------------------------
    juce::Reverb reverbHandler;
    juce::Label reverbMainLabel;

    // - Knobs -
    juce::Slider roomSizeKnob, dampingKnob, wetLevelKnob, dryLevelKnob, revWidthKnob, freezeModeKnob;
    
    // - Labels -
    juce::Label roomSizeLabel, dampingLabel, wetLevelLabel, dryLevelLabel, revWidthLabel, freezeModeLabel;
    
    juce::Slider* reverbSliders[6] = { &roomSizeKnob, &dampingKnob, &wetLevelKnob, &dryLevelKnob, &revWidthKnob, &freezeModeKnob };
    juce::Label* reverbLabels[6] = { &roomSizeLabel, &dampingLabel, &wetLevelLabel, &dryLevelLabel, &revWidthLabel, &freezeModeLabel };
    juce::String reverbLabelNames[6] = { "Room Size", "Damping", "Wet level", "Dry level", "Width", "Freeze Mode" };

    EffectUIBlock reverbBlock = {
        reverbSliders,
        reverbLabels,
        reverbLabelNames,
        6
    };

    // - Distortion ----------------------------------------------------------------------------------------------------------
    juce::Label distortionMainLabel;

    // - Knobs -
    juce::Slider driveKnob, mixKnob;

    // - Labels -
    juce::Label driveLabel, mixLabel;

    juce::Slider* distortionSliders[2] = { &driveKnob, &mixKnob };
    juce::Label* distortionLabels[2] = { &driveLabel, &mixLabel };
    juce::String distortionLabelNames[2] = { "Drive", "Mix" };

    EffectUIBlock distortionBlock = {
        distortionSliders,
        distortionLabels,
        distortionLabelNames,
        2
    };

    // - FUNCTIONS AND PROCEDURES ------------------------------------------------------------------------------------------------------------

    void setUpArea(std::function<void(juce::Component&)> addFn, int area, int posX, int posY);
    void retrieveData(int effectID, EffectUIBlock*& block);
    void setMainLabel(std::function<void(juce::Component&)> addFn, int area, int posX, int posY);

};