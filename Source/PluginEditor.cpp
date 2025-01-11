/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
SunkAudioProcessorEditor::SunkAudioProcessorEditor (SunkAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p),
sinkAttachment(audioProcessor.apvts, "Is Sunk", sink)
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    juce::Image nullImage;
    addAndMakeVisible(sink);
    notSunk = juce::ImageCache::getFromMemory(BinaryData::unpressed_png, BinaryData::unpressed_pngSize);
    yesSunk = juce::ImageCache::getFromMemory(BinaryData::pressed_png, BinaryData::pressed_pngSize);
    sink.setImages(true, true, false, notSunk, 1.f, transparent, nullImage, 1.0f, transparent, yesSunk, 1.f, transparent, 0);
    sink.setClickingTogglesState(true);
    
    
    setSize (400, 300);
}

SunkAudioProcessorEditor::~SunkAudioProcessorEditor()
{
}

//==============================================================================
void SunkAudioProcessorEditor::paint (juce::Graphics& g)
{
    bg = juce::ImageCache::getFromMemory(BinaryData::bg_png, BinaryData::bg_pngSize);
    g.drawImageWithin(bg, 0, 0, getWidth(), getHeight(), juce::RectanglePlacement::stretchToFit);
}

void SunkAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
    sink.setBounds((getLocalBounds().getWidth() - 300) / 2, (getLocalBounds().getHeight() - 300) / 2, 300, 300);
}

