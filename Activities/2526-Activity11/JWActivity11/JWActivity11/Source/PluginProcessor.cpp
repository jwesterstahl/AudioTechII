/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
JWActivity11AudioProcessor::JWActivity11AudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       ),
#endif
    apvts(*this, nullptr, "Parameters", createParams())
{
}


juce::AudioProcessorValueTreeState::ParameterLayout JWActivity11AudioProcessor::createParams()
{
    return {
        std::make_unique<juce::AudioParameterFloat>(juce::ParameterID{"freq", 1}, "Frequency", juce::NormalisableRange<float>(20.0f, 2000.0f, 1.0f, 0.3f), 440.0f),
        std::make_unique<juce::AudioParameterFloat>(juce::ParameterID{"amp", 1}, "Amplitude", 0.0f, 1.0f, 0.5f),
        std::make_unique<juce::AudioParameterBool>(juce::ParameterID{"mute", 1}, "Mute", false)
    };
}

JWActivity11AudioProcessor::~JWActivity11AudioProcessor()
{
}

//==============================================================================
const juce::String JWActivity11AudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool JWActivity11AudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool JWActivity11AudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool JWActivity11AudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double JWActivity11AudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int JWActivity11AudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int JWActivity11AudioProcessor::getCurrentProgram()
{
    return 0;
}

void JWActivity11AudioProcessor::setCurrentProgram (int index)
{
}

const juce::String JWActivity11AudioProcessor::getProgramName (int index)
{
    return {};
}

void JWActivity11AudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void JWActivity11AudioProcessor::prepareToPlay (double sampleRate, int numSamplesPerBlock)
{
    // You need to initialize your variables here!
    samplingRate = sampleRate;
    samplesPerBlock = numSamplesPerBlock;
    
    freq = 440;
    amp = 1;
    phase = 0;
    
    // envelope length in samples
    envSamples = samplingRate * int(envSec);
    
    envTracker = 0;
    
    
    freq = 440;
    amp = 1;
    phase = 0;
    
    // envelope length in samples
    envSamples = samplingRate * int(envSec);
    
    envTracker = 0;
}

void JWActivity11AudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool JWActivity11AudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
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

void JWActivity11AudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    // In case we have more outputs than inputs, this code clears any output
    // channels that didn't contain input data, (because these aren't
    // guaranteed to be empty - they may contain garbage).
    // This is here to avoid people getting screaming feedback
    // when they first compile a plugin, but obviously you don't need to keep
    // this code if your algorithm always overwrites all the output channels.
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    genSineWave(buffer);
    applyEnvRamp(buffer);
}

void JWActivity11AudioProcessor::genSineWave
 (juce::AudioBuffer<float>& buffer) {
    
    // Fill the buffer (in place) with a sinusoid
    // your code goes here!
    
    float phaseStart = phase;
    
    for (int channel = 0; channel < buffer.getNumChannels(); ++channel) {
        
        auto* channelData = buffer.getWritePointer (channel);
        phase = phaseStart;
        
        for (int i = 0; i < samplesPerBlock; i++) {
            
            channelData[i] = amp * sinf(phase);
            
            phase += juce::MathConstants<float>::twoPi * freq/samplingRate;
            
            if (phase >= juce::MathConstants<float>:: twoPi) {
                phase -= juce::MathConstants<float>:: twoPi;
            }
            
            
        }
    }
    
}



void JWActivity11AudioProcessor::applyEnvRamp(juce::AudioBuffer<float>& buffer)
{
    // Apply an amplitude envelope to the buffer (in place)
    // your code goes here!
    
    envSamples = envSec * samplingRate;
    
    float envStart = envTracker;
    float envVal;
    float halfEnvLen = float(envSamples) / 2;
    
    for (int channel = 0; channel < buffer.getNumChannels(); ++channel) {
        auto* channelData = buffer.getWritePointer(channel);
        envTracker = envStart;
        
        for (int i = 0; i < samplesPerBlock; i++ ) {
            
            
            if (envTracker < halfEnvLen){
                envVal = envTracker / halfEnvLen;
            }
            
            
            else {
                envVal = 1 - (envTracker - halfEnvLen) / halfEnvLen;
            }
            channelData[i] *= envVal;
            
            envTracker++;
            
            if (envTracker >= envSamples) {
                envTracker = 0;
            }
        }
    }
}

//==============================================================================
bool JWActivity11AudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* JWActivity11AudioProcessor::createEditor()
{
    return new JWActivity11AudioProcessorEditor (*this);
}

//==============================================================================
void JWActivity11AudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void JWActivity11AudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new JWActivity11AudioProcessor();
}
