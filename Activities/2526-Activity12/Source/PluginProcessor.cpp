/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
_2526Activity12AudioProcessor::_2526Activity12AudioProcessor()
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

_2526Activity12AudioProcessor::~_2526Activity12AudioProcessor()
{
}

juce::AudioProcessorValueTreeState::ParameterLayout _2526Activity12AudioProcessor::createParams()
{
    return {
        std::make_unique<juce::AudioParameterFloat>(juce::ParameterID{"delay", 1}, "Delay length", 0.0, 5, 0.25),
        std::make_unique<juce::AudioParameterFloat>(juce::ParameterID{"mix", 1}, "Dry/Wet Mix", 0.0, 1.0, 0.5)
    };
    
}

//==============================================================================
const juce::String _2526Activity12AudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool _2526Activity12AudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool _2526Activity12AudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool _2526Activity12AudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double _2526Activity12AudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int _2526Activity12AudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int _2526Activity12AudioProcessor::getCurrentProgram()
{
    return 0;
}

void _2526Activity12AudioProcessor::setCurrentProgram (int index)
{
}

const juce::String _2526Activity12AudioProcessor::getProgramName (int index)
{
    return {};
}

void _2526Activity12AudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void _2526Activity12AudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // initialize your variables here!
    samplingRate = sampleRate;
    bufferSize = samplesPerBlock;
    
    delayBufferSize = (int) (sampleRate * maxDelayLength);
    
    int numChannels = getTotalNumOutputChannels();
    delayBuffer.setSize(numChannels, delayBufferSize);
    delayBuffer.clear();
    
    writeTail = 0;
    
}

void _2526Activity12AudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool _2526Activity12AudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
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

void _2526Activity12AudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
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

    delay(buffer);
}

void _2526Activity12AudioProcessor::delay(juce::AudioBuffer<float> &buffer)
{
    // add your delay implementation here!
        
        auto* delayParam = apvts.getRawParameterValue("delay");
        auto delayLengthSec = delayParam->load();
    
        auto* mixParam = apvts.getRawParameterValue("mix");
        auto mix = mixParam->load();
        
        int delayInSamples = (int) (delayLengthSec * samplingRate);
        
    for (int channel = 0; channel < getTotalNumInputChannels(); ++channel)
        {
            float* channelData = buffer.getWritePointer(channel);
            float* delayData = delayBuffer.getWritePointer(channel);
                
            for (int i = 0; i < buffer.getNumSamples(); ++i)
            {
                int writeIndex = (writeTail + i) % delayBufferSize;
                
                int readIndex = (writeIndex - delayInSamples + delayBufferSize) % delayBufferSize;
                    
                float input = channelData[i];
                float delayed = delayData[readIndex];
                    
                delayData[writeIndex] = input;
                    
        
                channelData[i] = (input * (1.0f - mix)) + (delayed * mix);
            }
        }
        
        writeTail = (writeTail + bufferSize) % delayBufferSize;
}

//==============================================================================
bool _2526Activity12AudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* _2526Activity12AudioProcessor::createEditor()
{
    return new juce::GenericAudioProcessorEditor (*this);
}

//==============================================================================
void _2526Activity12AudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void _2526Activity12AudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new _2526Activity12AudioProcessor();
}
