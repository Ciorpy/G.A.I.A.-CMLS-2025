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
    setupOSC(); // Setup OSC (Needed in order to receive messages)
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

    reverbHandler.reset();

    dw = 0;
    dr = 1;
    ds = 50000;

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

    processReverb(channelOutDataL, channelOutDataR, reverb, numSamples);

    for (int i = 0; i < numSamples; ++i)
    {
        float inputL = channelInDataL[i];
        float inputR = channelInDataR[i];
        float sampleL = inputL;
        float sampleR = inputR;

        //Effects
        processDistortion(&sampleL, &sampleR, distortion);
        processDelay(&sampleL, &sampleR, distortion);
        processOctaver(&sampleL, &sampleR, distortion);

        channelOutDataL[i] = inputL + sampleL;
        channelOutDataR[i] = inputR + sampleR;

        dw = (dw + 1) % ds;
        dr = (dr + 1) % ds;
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

// Setup functions of OSC
void CMLSPROJECTJUCEAudioProcessor::setupOSC()
{
    //Tries to connect to door 9002
    if (!connect(9002))
    {
        // Connection failed
        DBG("Errore: porta OSC già in uso o non disponibile");
    }
    else 
    {
        //Connection successful
        DBG("PORTA APERTA");
    }

    //Adds listeners that receive the parameters of the effects from Processing
    juce::OSCReceiver::addListener(this, "/delay");
    juce::OSCReceiver::addListener(this, "/reverb");
    juce::OSCReceiver::addListener(this, "/distortion");
    juce::OSCReceiver::addListener(this, "/octaver");
}

//Callback function that handles the received messages and manages them
void CMLSPROJECTJUCEAudioProcessor::oscMessageReceived(const juce::OSCMessage& message)
{
    //Checks validity of the OSC msg
    if (message.size() == 1 && message[0].isFloat32()) {
        juce::String address = message.getAddressPattern().toString(); // recognizes param
        float value = std::floor(message[0].getFloat32() * 100.0f) / 100.0f; // gets value of MSG

        // Checks address and assign value to correct variable
        if (address == "/delay") 
        {
            delay = static_cast<float>(value);
            DBG("DELAY: " << delay);
        }
        else if (address == "/distortion") 
        {
            distortion = static_cast<float>(value);
            DBG("DISTORTION: " << distortion);
        }
        else if (address == "/reverb") 
        {
            reverb = static_cast<float>(value);
            DBG("REVERB: " << reverb);
		}
		else if (address == "/octaver")
		{
			octaver = static_cast<int>(value);
			DBG("OCTAVER: " << octaver);
		}
        else
        {
            DBG("OSC not valid");
        }
    }

    return;
}

void CMLSPROJECTJUCEAudioProcessor::processDelay(float* sampleL, float* sampleR, float delayVal)
{
    if (delayVal != 0.0f) {

    }

    return;
}


void CMLSPROJECTJUCEAudioProcessor::processReverb(float* left, float* right, float reverbVal, int numSamples)
{
    juce::Reverb::Parameters reverbParams = {1.0f, 0.0f, reverbVal, (1-reverbVal), 1.0f, 0.8f};

    reverbHandler.setParameters(reverbParams);
    reverbHandler.processStereo(left, right, numSamples);
    
    return;
}

void CMLSPROJECTJUCEAudioProcessor::processDistortion(float* sampleL, float* sampleR, float distortionVal)
{
    if (distortionVal != 0.0f) {
        *sampleL = 2.0f * distortionVal / juce::MathConstants<float>::pi * atan(*sampleL * 10);
        *sampleR = 2.0f * distortionVal / juce::MathConstants<float>::pi * atan(*sampleR * 10);
    }

    return;
}

void CMLSPROJECTJUCEAudioProcessor::processOctaver(float* sampleL, float* sampleR, int octaverVal)
{
    if (octaverVal != 0) {

    }

    return;
}