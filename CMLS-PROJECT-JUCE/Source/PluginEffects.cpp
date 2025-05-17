/*
  ==============================================================================

    PluginEffects.cpp
    Created: 16 May 2025 11:06:40am
    Author:  Marco65701

  ==============================================================================
*/

#include <JuceHeader.h>
#include "PluginEffects.h"


void CMLSPROJECTJUCEEffects::setupMixerUI(std::function<void(juce::Component&)> addFn)
{
    setUpArea(addFn, Reverb, 0, 0);
    //setUpArea(addFn, Distortion, distortionBlock);
    //setUpArea(addFn, Distortion, distortionBlock);
    //setUpArea(addFn, Distortion, distortionBlock);
    //setUpArea(addFn, Distortion, distortionBlock);
    //setUpArea(addFn, Distortion, distortionBlock);
}

void CMLSPROJECTJUCEEffects::setUpArea(std::function<void(juce::Component&)> addFn, int effectID, int posX, int posY)
{
    EffectUIBlock* block;

    retrieveData(effectID, block);
    setMainLabel(addFn, effectID, posX, posY);


    int sliderWidth = 100;
    int sliderHeight = 100;
    int spacing = (560 - block->numParams * sliderWidth) / (block->numParams-1);

    
    for (int i = 0; i < block->numParams; ++i)
    {
        auto& slider = block->sliders[i];
        auto& label = block->labels[i];
        auto val = 0.5f;
        float minVal = 0.0, maxVal = 1.0, step = 0.01;

        slider->setSliderStyle(juce::Slider::Rotary);
        slider->setTextBoxStyle(juce::Slider::TextBoxBelow, false, 50, 20);
        slider->setRange(minVal, maxVal, step);
        slider->setValue(val, juce::dontSendNotification);
        slider->onValueChange = [slider, i]() {
            DBG("Slider " << i << " value changed to: " << slider->getValue());
            };

        label->setText(block->labelNames[i], juce::dontSendNotification);
        label->attachToComponent(slider, false);

        addFn(*slider);
        addFn(*label);

        int x = posX + 20 + i * (sliderWidth + spacing);
        int y = posY + sliderHeight;
        slider->setBounds(x, y, sliderWidth, sliderHeight);
        label->setBounds(x, y - 15, sliderWidth, 20);
        label->setJustificationType(juce::Justification::centred);
    }
}


void CMLSPROJECTJUCEEffects::processDelay(float* sampleL, float* sampleR, float delayVal)
{
    if (delayVal != 0.0f) {

    }

    return;
}


void CMLSPROJECTJUCEEffects::processReverb(float* left, float* right, int numSamples)
{
    float roomSize = reverbBlock.sliders[0]->getValue();
    float damping = reverbBlock.sliders[1]->getValue();
    float wetLevel = reverbBlock.sliders[2]->getValue();
    float dryLevel = reverbBlock.sliders[3]->getValue();
    float revWidth = reverbBlock.sliders[4]->getValue();
    float freezeMode = reverbBlock.sliders[5]->getValue();

    DBG("roomSize" << roomSize);

    juce::Reverb::Parameters reverbParams = { roomSize, damping, wetLevel, dryLevel, revWidth, freezeMode};


    reverbHandler.setParameters(reverbParams);
    reverbHandler.processStereo(left, right, numSamples);
    return;
}

void CMLSPROJECTJUCEEffects::processDistortion(float* sampleL, float* sampleR, float distortionVal)
{
    if (distortionVal != 0.0f) {
        *sampleL = (distortionVal) / juce::MathConstants<float>::pi * atan(*sampleL * 10);
        *sampleR = (distortionVal) / juce::MathConstants<float>::pi * atan(*sampleR * 10);
    }

    return;
}

void CMLSPROJECTJUCEEffects::processOctaver(float* sampleL, float* sampleR, int octaverVal)
{
    if (octaverVal != 0) {

    }

    return;
}

void CMLSPROJECTJUCEEffects::setMainLabel(std::function<void(juce::Component&)> addFn, int effectID, int posX, int posY)
{
    juce::String mainLabelTitle = "sampleText";
    juce::Label* mainLabel = NULL;


    switch (static_cast<EffectID>(effectID))
    {
    case EffectID::Distortion:
        mainLabel = NULL;
        mainLabelTitle = "Distortion";
        break;

    case EffectID::Delay:
        mainLabel = NULL;
        mainLabelTitle = "Delay";
        break;

    case EffectID::Reverb:
        mainLabel = &reverbMainLabel;
        mainLabelTitle = "Reverb";
        break;

    case EffectID::Octaver:
        mainLabel = NULL;
        mainLabelTitle = "Octaver";
        break;

    case EffectID::WahWah:
        mainLabel = NULL;
        mainLabelTitle = "Wah-wah";
        break;

    case EffectID::Envelope:
        mainLabel = NULL;
        mainLabelTitle = "Envelope";
        break;

    default:
        jassertfalse; // Errore: valore sconosciuto
        break;
    }
    
    if (mainLabel != NULL) {
        mainLabel->setBounds(posX, posY, 600, 60);
        mainLabel->setText(mainLabelTitle, juce::dontSendNotification);
        mainLabel->setFont(juce::Font(30));
        mainLabel->setJustificationType(juce::Justification::centred);
        addFn(*mainLabel);
    }
}

void CMLSPROJECTJUCEEffects::retrieveData(int effectID, EffectUIBlock*& block)
{
    block = NULL;


    switch (static_cast<EffectID>(effectID))
    {
    case EffectID::Distortion:

        break;

    case EffectID::Delay:
        break;

    case EffectID::Reverb:
        block = &reverbBlock;
        break;

    case EffectID::Octaver:
        break;

    case EffectID::WahWah:
        break;

    case EffectID::Envelope:
        break;

    default:
        block = NULL;
        jassertfalse; // Errore: valore sconosciuto
        break;
    }
}