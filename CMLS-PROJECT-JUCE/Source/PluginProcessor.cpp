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
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
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
bool CMLSPROJECTJUCEAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void CMLSPROJECTJUCEAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer&)
{
    int numSamples = buffer.getNumSamples();
    auto totalNumInputChannels = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear(i, 0, numSamples);

    float* channelOutDataL = buffer.getWritePointer(0);
    float* channelOutDataR = buffer.getWritePointer(1);
    const float* channelInDataL = buffer.getReadPointer(0);
    const float* channelInDataR = buffer.getReadPointer(1);

    for (int i = 0; i < numSamples; ++i)
    {
        float inputL = channelInDataL[i];
        float inputR = channelInDataR[i];
        float sampleL = inputL;
        float sampleR = inputR;

        //Effects
        processDistortion(&sampleL, &sampleR);
        //processDelay(&sampleL, &sampleR, distortion);
        //processOctaver(&sampleL, &sampleR, distortion);

        channelOutDataL[i] = sampleL;
        channelOutDataR[i] = sampleR;

        dw = (dw + 1) % ds;
        dr = (dr + 1) % ds;
    }

    processReverb(channelOutDataL, channelOutDataR, numSamples);
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