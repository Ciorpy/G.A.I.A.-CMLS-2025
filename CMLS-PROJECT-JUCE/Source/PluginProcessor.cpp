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

    dw = 0;
    dr = 1;
    //ds = 50000;

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

    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear(i, 0, numSamples);

    float* channelOutDataL1 = buffer.getWritePointer(0);
    float* channelOutDataR1 = buffer.getWritePointer(1);
    float* channelOutDataL2 = buffer.getWritePointer(2);
    float* channelOutDataR2 = buffer.getWritePointer(3);
    float* channelOutDataL3 = buffer.getWritePointer(4);
    float* channelOutDataR3 = buffer.getWritePointer(5);
    const float* channelInDataL1 = buffer.getReadPointer(0);
    const float* channelInDataR1 = buffer.getReadPointer(1);
    const float* channelInDataL2 = buffer.getReadPointer(2);
    const float* channelInDataR2 = buffer.getReadPointer(3);
    const float* channelInDataL3 = buffer.getReadPointer(4);
    const float* channelInDataR3 = buffer.getReadPointer(5);

    for (int i = 0; i < numSamples; ++i)
    {
        float inputL1 = channelInDataL1[i];
        float inputR1 = channelInDataR1[i];
        float sampleL1 = inputL1;
        float sampleR1 = inputR1;

        float inputL2 = channelInDataL2[i];
        float inputR2 = channelInDataR2[i];
        float sampleL2 = inputL2;
        float sampleR2 = inputR2;

        float inputL3 = channelInDataL3[i];
        float inputR3 = channelInDataR3[i];
        float sampleL3 = inputL3;
        float sampleR3 = inputR3;

        //Effects
        processDistortion(&sampleL1, &sampleR1, 0);
        processDistortion(&sampleL2, &sampleR2, 1);
        processDistortion(&sampleL3, &sampleR3, 2);
        //processDelay(&sampleL, &sampleR, distortion);
        //processOctaver(&sampleL, &sampleR, distortion);

        channelOutDataL1[i] = sampleL1;
        channelOutDataR1[i] = sampleR1;
        channelOutDataL2[i] = sampleL2;
        channelOutDataR2[i] = sampleR2;
        channelOutDataL3[i] = sampleL3;
        channelOutDataR3[i] = sampleR3;

        dw = (dw + 1) % ds;
        dr = (dr + 1) % ds;
    }

    processReverb(channelOutDataL1, channelOutDataR1, numSamples, 0);
    processReverb(channelOutDataL1, channelOutDataR1, numSamples, 1);
    processReverb(channelOutDataL1, channelOutDataR1, numSamples, 2);
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