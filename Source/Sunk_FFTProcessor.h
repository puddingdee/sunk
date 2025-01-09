//
//  Sunk_FFTProcessor.h
//  sunk
//
//  Created by Merritt Hyman on 1/7/25.
//

#pragma once

#include <JuceHeader.h>

class Sunk_FFTProcessor
{
public:
    Sunk_FFTProcessor();
    
    int getLatencyInSamples() const { return fftSize; };
    void reset();
    float processSample(float sample, bool bypassed, bool isSunk);
    void processBlock(float* data, int numSamples, bool bypassed, bool isSunk);
private:
    void processFrame(bool bypasssed, bool isSunk);
    void processSpectrum(float* data, int numSamples);
    //constants
    static constexpr int fftOrder = 10;
    static constexpr int fftSize = 1 << fftOrder; // 1024 samples
    static constexpr int numBins = fftSize / 2 + 1; // 513 bins
    static constexpr int overlap = 4; // 75% overlap
    static constexpr int hopSize = fftSize / overlap; // 256 samples
   
    static constexpr float windowCorrection = 2.0f / 3.0f;
    
    juce::dsp::FFT fft;
    juce::dsp::WindowingFunction<float> window;
    
    // FIFO vars
    int count = 0;
    int pos = 0;
    
    // circular buffers for in and out
    std::array<float, fftSize> inputFifo;
    std::array<float, fftSize> outputFifo;
    
    //FFT workspace. contains interleaved complex numbers
    std::array<float, fftSize * 2> fftData;
   
    //sunk vars
    float playbackRate = 1.0f;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Sunk_FFTProcessor)
};
