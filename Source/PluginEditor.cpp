/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
SunkAudioProcessorEditor::SunkAudioProcessorEditor (SunkAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    
    initWindow();
    
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
}
