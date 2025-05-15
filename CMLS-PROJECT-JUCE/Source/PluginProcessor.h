/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

//==============================================================================
/**
*/

/* Class CMLSPROJECTJUCEAudioProcessor inherits from AudioProcessor, OSCReceiver and
   OSCReceiver::ListenerWithOSCAddress<juce::OSCReceiver::MessageLoopCallback>. This allows 
   us to gain access to functions needed to allow comunication via OSC protocol*/
class CMLSPROJECTJUCEAudioProcessor  : public juce::AudioProcessor, private juce::OSCReceiver,
    private juce::OSCReceiver::ListenerWithOSCAddress<juce::OSCReceiver::MessageLoopCallback>
{
public:
    //==============================================================================
    CMLSPROJECTJUCEAudioProcessor();
    ~CMLSPROJECTJUCEAudioProcessor() override;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;
private:
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (CMLSPROJECTJUCEAudioProcessor)

    // Functions related to OSC
    void setupOSC(); // Setup function
    void oscMessageReceived(const juce::OSCMessage& message) override; //Override of callback function
    
    juce::Reverb reverbHandler;

    // Audio processing
    juce::AudioSampleBuffer dbuf;
    int dw;
    int dr;

    // Variables related to effects
    float wet;
    float dry;
    int ds = 44100;

    float delay = 0.0f;
    float reverb = 0.0f;
    float distortion = 0.0f;
    int octaver = 0;

    void processDelay(float* sampleL, float* sampleR, float delayVal);
    void processReverb(float* left, float* right, float reverbVal, int numSamples);
    void processDistortion(float* sample, float* sampleR, float distortionVal);
    void processOctaver(float* sample, float* sampleR, int octaverVal);
};
