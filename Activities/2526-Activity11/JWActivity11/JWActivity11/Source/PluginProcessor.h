/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

class JWActivity11AudioProcessor  : public juce::AudioProcessor
{
public:
    JWActivity11AudioProcessor();
    ~JWActivity11AudioProcessor() override;

    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;
    const juce::String getName() const override;
    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;
    
    juce::AudioProcessorValueTreeState apvts;

    // Function to trigger envelope from the Editor ---
    void triggerEnvelope() { envelopeTriggered = true; }

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (JWActivity11AudioProcessor)
    float samplingRate;
    int samplesPerBlock;
    
    float freq;
    float amp;
    float phase;
    
    const float envSec = 1.0;
    int envSamples;
    int envTracker = 0;
    
    // --- NEW: Thread-safe flag for the trigger ---
    std::atomic<bool> envelopeTriggered { false };
    
    void genSineWave(juce::AudioBuffer<float>& buffer);
    void applyEnvRamp(juce::AudioBuffer<float>& buffer);
    
    juce::AudioProcessorValueTreeState::ParameterLayout createParams();
};
