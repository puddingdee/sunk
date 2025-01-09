/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"
//==============================================================================
SunkAudioProcessor::SunkAudioProcessor()
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
}

SunkAudioProcessor::~SunkAudioProcessor()
{
}

//==============================================================================
const juce::String SunkAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool SunkAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool SunkAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool SunkAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double SunkAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int SunkAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int SunkAudioProcessor::getCurrentProgram()
{
    return 0;
}

void SunkAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String SunkAudioProcessor::getProgramName (int index)
{
    return {};
}

void SunkAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void SunkAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
    setLatencySamples(fft[0].getLatencyInSamples());
    
    fft[0].reset();
    fft[1].reset();
}

void SunkAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

bool SunkAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
    // Allow only stereo input and output
        if (layouts.getMainInputChannelSet() != juce::AudioChannelSet::stereo()
            || layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
            return false;

        return true;
}

void SunkAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto numInputChannels  = getTotalNumInputChannels();
    auto numOutputChannels = getTotalNumOutputChannels();
    auto numSamples = buffer.getNumSamples();
    
    for (auto i = numInputChannels; i < numOutputChannels; ++i){
        buffer.clear(i, 0, numSamples);
    }

    bool bypassed = apvts.getRawParameterValue("Bypassed")->load();
    bool isSunk = apvts.getRawParameterValue("Is Sunk")->load();
    
    float* channelL = buffer.getWritePointer(0);
    float* channelR = buffer.getWritePointer(1);
    
    for (int sample = 0; sample < numSamples; ++sample){
        float sampleL = channelL[sample];
        float sampleR = channelR[sample];
        
        sampleL = fft[0].processSample(sampleL, bypassed, isSunk);
        sampleR = fft[1].processSample(sampleR, bypassed, isSunk);
        
        channelL[sample] = sampleL;
        channelR[sample] = sampleR;
        /*
        for (int channel = 0; channel < numInputChannels; ++channel) {
                auto* channelData = buffer.getWritePointer(channel);
                fft[channel].processBlock(channelData, numSamples, bypassed);
            }
        */
    }
   
}
juce::AudioProcessorValueTreeState::ParameterLayout SunkAudioProcessor::createParameterLayout()
{
    juce::AudioProcessorValueTreeState::ParameterLayout layout;
    layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("Speed", 1), "Speed", juce::NormalisableRange<float>(-10.0f, 10.0f, 0.1f, 1.0f), 0.0f));
    
    layout.add(std::make_unique<juce::AudioParameterBool>(juce::ParameterID("Is Sunk", 1), "Is Sunk", false));
    layout.add(std::make_unique<juce::AudioParameterBool>(juce::ParameterID("Bypassed", 1), "Bypassed", false));

    return layout;
}
 
//==============================================================================
bool SunkAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* SunkAudioProcessor::createEditor()
{
    //return new SunkAudioProcessorEditor (*this);
    return new juce::GenericAudioProcessorEditor(*this);
}

//==============================================================================
void SunkAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
    copyXmlToBinary(*apvts.copyState().createXml(), destData);
}

void SunkAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
    std::unique_ptr<juce::XmlElement> xml(getXmlFromBinary(data, sizeInBytes));
    if (xml.get() != nullptr && xml->hasTagName(apvts.state.getType())){
        apvts.replaceState(juce::ValueTree::fromXml(*xml));
    }
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new SunkAudioProcessor();
}
