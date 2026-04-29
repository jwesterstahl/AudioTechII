/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

class JWActivity11AudioProcessorEditor  : public juce::AudioProcessorEditor
{
public:
    JWActivity11AudioProcessorEditor (JWActivity11AudioProcessor&);
    ~JWActivity11AudioProcessorEditor() override;

    void paint (juce::Graphics&) override;
    void resized() override;

private:
    JWActivity11AudioProcessor& audioProcessor;
    
    // gui
    juce::Slider freqSlider;
    juce::Slider ampSlider;
    juce::Label freqLabel{ "FreqLabel", "Frequency" };
    juce::Label ampLabel{ "AmpLabel", "Amplitude" };
    
    juce::ToggleButton muteButton{ "Mute" };
    juce::TextButton envTriggerButton{ "Trigger Envelope" };
    
    // apvts logic
    using SliderAttachment = juce::AudioProcessorValueTreeState::SliderAttachment;
    using ButtonAttachment = juce::AudioProcessorValueTreeState::ButtonAttachment;
    
    std::unique_ptr<SliderAttachment> freqAttach;
    std::unique_ptr<SliderAttachment> ampAttach;
    std::unique_ptr<ButtonAttachment> muteAttach;
    
    juce::GroupComponent oscGroup{"oscGroup", "Oscillator"};
    juce::GroupComponent controlGroup{"controlGroup", "Controls"};
    
    void styleSlider(juce::Slider& s);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (JWActivity11AudioProcessorEditor)
};

namespace ColoursTheme
{
    const juce::Colour background = juce::Colour(0xff121212);
    const juce::Colour panel = juce::Colour(0xff1e1e1e);
    const juce::Colour accent = juce::Colour(0xff4dd0e1);
    const juce::Colour text = juce::Colours::white;
    const juce::Colour knob = juce::Colour(0xff2a2a2a);
}
