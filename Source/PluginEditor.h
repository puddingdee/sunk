/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

//==============================================================================
/**
*/
class SunkAudioProcessorEditor  : public juce::AudioProcessorEditor
{
public:
    SunkAudioProcessorEditor (SunkAudioProcessor&);
    ~SunkAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;
    
    void toggleState();
    void parameterChanged(const std::string& parameterID, float newValue);

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    SunkAudioProcessor& audioProcessor;
    void initWindow();
    
    
    juce::Image bg;
    juce::Image notSunk;
    juce::Image yesSunk;
    juce::ImageButton sink;
    juce::Colour transparent = juce::Colour::fromRGBA(0, 0, 0, 0);

    using APVTS = juce::AudioProcessorValueTreeState;
    using Attachment = APVTS::ButtonAttachment;
    
    Attachment sinkAttachment;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SunkAudioProcessorEditor)
    
    
};
