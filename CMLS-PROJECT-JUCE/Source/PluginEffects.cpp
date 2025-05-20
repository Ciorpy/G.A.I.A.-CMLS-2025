/*
  ==============================================================================

    PluginEffects.cpp
    Created: 16 May 2025 11:06:40am
    Author:  Marco65701

  ==============================================================================
*/

#include <JuceHeader.h>
#include "PluginEffects.h"


int CMLSPROJECTJUCEEffects::getDelayDS(int SuperCollComponent)
{
    int v = delayBlock.sliders[2][SuperCollComponent].getValue();
    if (v <= 0)
        v = 50000;
    DBG("getDelayDS(" << SuperCollComponent << ") = " << v);
    return v;
}

void CMLSPROJECTJUCEEffects::setupMixerUI(std::function<void(juce::Component&)> addFn)
{
    setUpArea(addFn, Reverb, 0, 0);
    setUpArea(addFn, Distortion, 0, 300);
    setUpArea(addFn, Delay, 0, 600);
    //setUpArea(addFn, Distortion, distortionBlock);
    //setUpArea(addFn, Distortion, distortionBlock);
    //setUpArea(addFn, Distortion, distortionBlock);
}

void CMLSPROJECTJUCEEffects::setUpArea(std::function<void(juce::Component&)> addFn, int effectID, int posX, int posY)
{
    EffectUIBlock* block;
    int margin = 0;


    retrieveData(effectID, block);

    switch (block->numParams) {
    case 2:
        margin = 180;
        break;
    case 3:
        margin = 120;
        break;
    case 4:
        margin = 80;
        break;
    case 5:
        margin = 40;
        break;
    case 6:
        margin = 20;
        break;
    default:
        margin = 0;
        break;
    }

    int sliderWidth = 100;
    int sliderHeight = 100;
    int spacing = (600 - margin * 2 - block->numParams * sliderWidth) / (block->numParams-1);
    int nextBlockSpacingX = 600;
    bool isMiddleCol = false;

    for (int j = 0; j < 3; j++) {
        isMiddleCol = j % 2 == 1;

        if (isMiddleCol) {
            setMainLabel(addFn, effectID, posX + j * nextBlockSpacingX, posY);
        }

        for (int i = 0; i < block->numParams; ++i)
        {
            auto& slider = block->sliders[i][j];
            auto& label = block->labels[i][j];
            auto val = 0.5f;
            float minVal = 0.0, maxVal = 1.0, step = 0.01;

			if (effectID == Delay && i == block->numParams - 1)
			{
				minVal = 100.0f;
				maxVal = 5000.0f;
				step = 100.0f;
                val = 500.0f;
			}

            slider.setSliderStyle(juce::Slider::Rotary);
            slider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 50, 20);
            slider.setRange(minVal, maxVal, step);
            slider.setValue(val, juce::dontSendNotification);

            label.setText(block->labelNames[i], juce::dontSendNotification);
            label.attachToComponent(&slider, false);

            addFn(slider);
            addFn(label);

            int x = posX + nextBlockSpacingX * j + margin + i * (sliderWidth + spacing);
            int y = posY + sliderHeight;
            slider.setBounds(x, y, sliderWidth, sliderHeight);
            label.setBounds(x, y - 15, sliderWidth, 20);
            label.setJustificationType(juce::Justification::centred);
        }
    }
}


void CMLSPROJECTJUCEEffects::processDelay(float* left, float* right, int SuperCollComponent)
{
    int leftIndex = 0;
    int rightIndex = 0;
	switch (SuperCollComponent)
	{
	case 0:
        leftIndex = 0;
        rightIndex = 1;
		break;
	case 1:
        leftIndex = 2;
        rightIndex = 3;
		break;
	case 2:
        leftIndex = 4;
        rightIndex = 5;
		break;
	default:
		break;
	}

    float wetLevel = delayBlock.sliders[0][SuperCollComponent].getValue();
    float dryLevel = delayBlock.sliders[1][SuperCollComponent].getValue();

    dbuf.setSample(leftIndex, dw[SuperCollComponent], *left);
    dbuf.setSample(rightIndex, dw[SuperCollComponent], *right);

    *left = dryLevel * *left + wetLevel * dbuf.getSample(leftIndex, dr[SuperCollComponent]);
    *right = dryLevel * *right + wetLevel * dbuf.getSample(rightIndex, dr[SuperCollComponent]);
    return;
}


void CMLSPROJECTJUCEEffects::processReverb(float* left, float* right, int numSamples, int SuperCollComponent)
{
    float roomSize = reverbBlock.sliders[0][SuperCollComponent].getValue();
    float damping = reverbBlock.sliders[1][SuperCollComponent].getValue();
    float wetLevel = reverbBlock.sliders[2][SuperCollComponent].getValue();
    float dryLevel = reverbBlock.sliders[3][SuperCollComponent].getValue();
    float revWidth = reverbBlock.sliders[4][SuperCollComponent].getValue();
    float freezeMode = reverbBlock.sliders[5][SuperCollComponent].getValue();

    juce::Reverb::Parameters reverbParams = { roomSize, damping, wetLevel, dryLevel, revWidth, freezeMode};


    reverbHandler.setParameters(reverbParams);
    reverbHandler.processStereo(left, right, numSamples);
    return;
}

void CMLSPROJECTJUCEEffects::processDistortion(float* sampleL, float* sampleR, int SuperCollComponent)
{
    float drive = distortionBlock.sliders[0][SuperCollComponent].getValue();
    float mix = distortionBlock.sliders[1][SuperCollComponent].getValue();

    *sampleL = (1 - mix) * *sampleL + mix * juce::jlimit(-0.1f, 0.1f, *sampleL * drive);
    *sampleR = (1 - mix) * *sampleR + mix * juce::jlimit(-0.1f, 0.1f, *sampleR * drive);


    return;
}


void CMLSPROJECTJUCEEffects::setMainLabel(std::function<void(juce::Component&)> addFn, int effectID, int posX, int posY)
{
    juce::String mainLabelTitle = "sampleText";
    juce::Label* mainLabel = NULL;


    switch (static_cast<EffectID>(effectID))
    {
    case EffectID::Distortion:
        mainLabel = &distortionMainLabel;
        mainLabelTitle = "Distortion";
        break;

    case EffectID::Delay:
        mainLabel = &delayMainLabel;
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
		block = &distortionBlock;
        break;

    case EffectID::Delay:
        block = &delayBlock;
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