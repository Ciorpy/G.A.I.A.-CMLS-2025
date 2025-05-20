/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
CMLSPROJECTJUCEAudioProcessor::CMLSPROJECTJUCEAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::discreteChannels(6), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::discreteChannels(6), true)
                     #endif
                       )
#endif
{
}

CMLSPROJECTJUCEAudioProcessor::~CMLSPROJECTJUCEAudioProcessor()
{
}

//==============================================================================
const juce::String CMLSPROJECTJUCEAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool CMLSPROJECTJUCEAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool CMLSPROJECTJUCEAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool CMLSPROJECTJUCEAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double CMLSPROJECTJUCEAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int CMLSPROJECTJUCEAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int CMLSPROJECTJUCEAudioProcessor::getCurrentProgram()
{
    return 0;
}

void CMLSPROJECTJUCEAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String CMLSPROJECTJUCEAudioProcessor::getProgramName (int index)
{
    return {};
}

void CMLSPROJECTJUCEAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void CMLSPROJECTJUCEAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..


    dbuf.setSize(getTotalNumOutputChannels(), 100000);
    dbuf.clear();
	for (int i = 0; i < 3; ++i)
	{
		dr[i] = 0;
		dw[i] = 1;
	}

}

void CMLSPROJECTJUCEAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool CMLSPROJECTJUCEAudioProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const
{
    const auto& inputLayout = layouts.getMainInputChannelSet();
    const auto& outputLayout = layouts.getMainOutputChannelSet();

    // Accetta solo 6 canali discreti in input
    if (inputLayout != juce::AudioChannelSet::discreteChannels(6))
        return false;

    // Output deve essere stereo
    if (outputLayout != juce::AudioChannelSet::discreteChannels(6))
        return false;

    return true;
}
#endif

void CMLSPROJECTJUCEAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer&)
{
    int numSamples = buffer.getNumSamples();
    auto totalNumInputChannels = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    // Pulizia buffer
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear(i, 0, numSamples);

    // Riferimenti a write e read pointer del buffer
    float* channelOutData[6]{};
    const float* channelInData[6]{};

    for (int i = 0; i < totalNumInputChannels; i++) {
        channelOutData[i] = buffer.getWritePointer(i);
        channelInData[i] = buffer.getReadPointer(i);
    }

    // Variabile che permette la corretta applicazione dei delay
    int ds_now[3] = { getDelayDS(Chords), getDelayDS(Bass), getDelayDS(Notes) };

    // 
    int leftIndex = 0, rightIndex = 1;

    for (int i = 0; i < numSamples; ++i)
    {
        for (int j = 0; j < totalNumInputChannels / 2; j++) {
            leftIndex = j * 2;
            rightIndex = j * 2 + 1;

            float left = channelInData[leftIndex][i];
            float right = channelInData[rightIndex][i];

            // Copia input
            float processedLeft = left;
            float processedRight = right;

            // Applica effetti
            processDistortion(&processedLeft, &processedRight, j);
            processDelay(&processedLeft, &processedRight, j);

            // Scrivi nel buffer di output
            channelOutData[leftIndex][i] = processedLeft;
            channelOutData[rightIndex][i] = processedRight;

            dw[j] = (dw[j] + 1) % ds_now[j];
            dr[j] = (dr[j] + 1) % ds_now[j];
        }
    }

    for (int j = 0; j < totalNumInputChannels / 2; j++) {
        leftIndex = j * 2;
        rightIndex = j * 2 + 1;

        dr[j] = (dw[j] - ds_now[j] + 100000) % 100000;

        // A questo punto channelOutData[x] punta ancora al buffer originale
        processReverb(channelOutData[leftIndex], channelOutData[rightIndex], numSamples, j);
    }
}





//==============================================================================
bool CMLSPROJECTJUCEAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* CMLSPROJECTJUCEAudioProcessor::createEditor()
{
    return new CMLSPROJECTJUCEAudioProcessorEditor (*this);
}

//==============================================================================
void CMLSPROJECTJUCEAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void CMLSPROJECTJUCEAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new CMLSPROJECTJUCEAudioProcessor();
}