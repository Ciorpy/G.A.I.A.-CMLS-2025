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

void CMLSPROJECTJUCEAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    int numSamples = buffer.getNumSamples();
    auto totalNumInputChannels = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    // Pulisce eventuali canali output extra
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear(i, 0, buffer.getNumSamples());

    // === PARAMETRI EFFETTI ===
    float delay_now = delay;       // da 0.0 a 1.0 (controlla tempo di delay)
    float distortion_now = distortion;  // da 0.0 a 1.0
    float reverb_now = reverb;      // da 0.0 a 1.0
    float mix = 0.5f;        // 0.0 = dry, 1.0 = wet (fisso, o crea parametro)
    float dry_now = 1.0f - mix;
    float wet_now = mix;

    float* channelOutDataL = buffer.getWritePointer(0);
    float* channelOutDataR = buffer.getWritePointer(1);
    const float* channelInData = buffer.getReadPointer(0); // mono input (o stereo duplica)

    for (int i = 0; i < numSamples; ++i)
    {
        float inputSample = channelInData[i];

        // 1. Delay: scrivi input nel delay buffer
        dbuf.setSample(0, dw, inputSample);
        dbuf.setSample(1, dw, inputSample);

        float delayedL = dbuf.getSample(0, dr);
        float delayedR = dbuf.getSample(1, dr);

        // 2. Distorsione: soft clipping con tanh
        float drive = juce::jmap(distortion_now, 1.0f, 10.0f);  // mappa da 1x a 10x
        float distortedL = std::tanh(delayedL * drive);
        float distortedR = std::tanh(delayedR * drive);

        // 3. Riverbero semplice (feedback)
        float reverbGain = reverb_now * 0.5f;  // peso della coda
        float reverbL = distortedL + delayedL * reverbGain;
        float reverbR = distortedR + delayedR * reverbGain;

        // 4. Wet/Dry mix finale
        channelOutDataL[i] = dry_now * inputSample + wet_now * reverbL;
        channelOutDataR[i] = dry_now * inputSample + wet_now * reverbR;

        // 5. Avanza nel delay buffer circolare
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

// Setter of wet property
void CMLSPROJECTJUCEAudioProcessor::set_wet(float val)
{
    wet = val;
}

// Setter of dry property
void CMLSPROJECTJUCEAudioProcessor::set_dry(float val)
{
    dry = val;
}

//Setter of ds property
void CMLSPROJECTJUCEAudioProcessor::set_ds(int val)
{
    ds = val;
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
            delay = value;
            DBG("DELAY: " << delay);
        }
        else if (address == "/distortion") 
        {
            distortion = value;
            DBG("DISTORTION: " << distortion);
        }
        else if (address == "/reverb") 
        {
            reverb = value;
            DBG("REVERB: " << reverb);
        }
        else
        {
            DBG("OSC not valid");
        }
    }
}