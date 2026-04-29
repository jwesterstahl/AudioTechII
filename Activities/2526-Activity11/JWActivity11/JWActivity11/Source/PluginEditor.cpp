/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
JWActivity11AudioProcessorEditor::JWActivity11AudioProcessorEditor (JWActivity11AudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize (400, 300);

    // get apvts ref
    auto& apvts = audioProcessor.apvts;

    // quick set up for sliders
    auto setupSlider = [](juce::Slider& s)
        {
            s.setSliderStyle(juce::Slider::Rotary);
            s.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 20);
        };

    setupSlider(freqSlider);
    setupSlider(ampSlider);

    // components
    addAndMakeVisible(oscGroup);
    addAndMakeVisible(controlGroup);

    addAndMakeVisible(freqSlider);
    addAndMakeVisible(ampSlider);
    
    addAndMakeVisible(muteButton);
    addAndMakeVisible(envTriggerButton);

    // labels
    auto setupLabel = [](juce::Label& l, const juce::String& text)
        {
            l.setText(text, juce::dontSendNotification);
            l.setJustificationType(juce::Justification::centred);
            l.setColour(juce::Label::textColourId, ColoursTheme::text);
        };

    setupLabel(freqLabel, "Frequency");
    setupLabel(ampLabel, "Amplitude");

    addAndMakeVisible(freqLabel);
    addAndMakeVisible(ampLabel);
    
    // Envelope Button Action
    envTriggerButton.onClick = [this]() {
        audioProcessor.triggerEnvelope();
    };

    // attachments
    freqAttach = std::make_unique<SliderAttachment>(apvts, "freq", freqSlider);
    ampAttach = std::make_unique<SliderAttachment>(apvts, "amp", ampSlider);
    muteAttach = std::make_unique<ButtonAttachment>(apvts, "mute", muteButton);
}

JWActivity11AudioProcessorEditor::~JWActivity11AudioProcessorEditor()
{
}

//==============================================================================
void JWActivity11AudioProcessorEditor::paint (juce::Graphics& g)
{
    g.fillAll(ColoursTheme::background);

    auto area = getLocalBounds().reduced(10);

    auto top = area.removeFromTop(area.getHeight() / 2);

    g.setColour(ColoursTheme::panel);
    g.fillRoundedRectangle(top.toFloat(), 10.0f);
    g.fillRoundedRectangle(area.toFloat(), 10.0f);

    oscGroup.setColour(juce::GroupComponent::outlineColourId, ColoursTheme::accent);
    oscGroup.setColour(juce::GroupComponent::textColourId, ColoursTheme::text);

    controlGroup.setColour(juce::GroupComponent::outlineColourId, ColoursTheme::accent);
    controlGroup.setColour(juce::GroupComponent::textColourId, ColoursTheme::text);

    styleSlider(freqSlider);
    styleSlider(ampSlider);

    muteButton.setColour(juce::ToggleButton::textColourId, ColoursTheme::text);
    muteButton.setColour(juce::ToggleButton::tickColourId, ColoursTheme::accent);
    
    envTriggerButton.setColour(juce::TextButton::buttonColourId, ColoursTheme::knob);
    envTriggerButton.setColour(juce::TextButton::textColourOffId, ColoursTheme::accent);
}

void JWActivity11AudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..

    auto area = getLocalBounds().reduced(10);

    auto top = area.removeFromTop(area.getHeight() / 2);

    // Oscillator group
    oscGroup.setBounds(top);
    auto oscArea = top.reduced(10);

    freqSlider.setBounds(oscArea.removeFromLeft(100));
    ampSlider.setBounds(oscArea.removeFromLeft(100));

    // Controls group
    controlGroup.setBounds(area);
    auto controlArea = area.reduced(10);

    muteButton.setBounds(controlArea.removeFromLeft(100).reduced(10));
    
    // Add a little extra reduction to make the trigger button fit nicely
    envTriggerButton.setBounds(controlArea.removeFromLeft(150).reduced(15));

    // labels
    freqLabel.setBounds(freqSlider.getX(), freqSlider.getBottom(), 100, 20);
    ampLabel.setBounds(ampSlider.getX(), ampSlider.getBottom(), 100, 20);
}

void JWActivity11AudioProcessorEditor::styleSlider(juce::Slider& s)
{
    s.setColour(juce::Slider::rotarySliderFillColourId, ColoursTheme::accent);
    s.setColour(juce::Slider::thumbColourId, ColoursTheme::accent);
    s.setColour(juce::Slider::rotarySliderOutlineColourId, ColoursTheme::knob);
    s.setColour(juce::Slider::textBoxTextColourId, ColoursTheme::text);
    s.setColour(juce::Slider::textBoxOutlineColourId, juce::Colours::transparentBlack);
}
