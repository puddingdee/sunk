//
//  Sunk_FFTProcessor.cpp
//  sunk
//
//  Created by Merritt Hyman on 1/7/25.
//

#include "Sunk_FFTProcessor.h"

Sunk_FFTProcessor::Sunk_FFTProcessor() :
fft(fftOrder), window(fftSize + 1, juce::dsp::WindowingFunction<float>::WindowingMethod::hann, false)
{
    
}

void Sunk_FFTProcessor::reset()
{
    count = 0;
    pos = 0;
    
    //zero out FIFOs
    std::fill(inputFifo.begin(), inputFifo.end(), 0.0f);
    std::fill(outputFifo.begin(), outputFifo.end(), 0.0f);
}

void Sunk_FFTProcessor::processBlock(float* data, int numSamples, bool bypassed, bool isSunk)
{
    for (int i = 0; i < numSamples; ++i){
        data[i] = processSample(data[i], bypassed, isSunk);
    }
}


float Sunk_FFTProcessor::processSample(float sample, bool bypassed, bool isSunk)
{
    // push the new sample value into the input FIFO
    inputFifo[pos] = sample;
    // read the output value from the output FIFO. this is where latency equal to fftSize is introduced
    
    float outputSample = outputFifo[pos];
    //once the sample is read, set this position in the fifo back to zero so we can add the IFFT results to it later
    outputFifo[pos] = 0.0f;
    
    pos += 1;
    if (pos == fftSize){
        pos = 0;

    }
    count += 1;
    if (count == hopSize) {
        count = 0;
        processFrame(bypassed, isSunk);
        
    }
    
    return outputSample;
}

void Sunk_FFTProcessor::processFrame(bool bypassed, bool isSunk)
{
    static std::vector<float> frozenFFTData(fftSize, 0.0f); // Store frozen FFT data

    
    const float* inputPtr = inputFifo.data();
    float* fftPtr = fftData.data();
    

    //normal processing. copy input data to FFT buffer
    std::memcpy(fftPtr, inputPtr + pos, (fftSize - pos) * sizeof(float));
    if (pos > 0){
        std::memcpy(fftPtr + fftSize - pos, inputPtr, pos * sizeof(float));
    }
    
    // apply the window to avoid spectral leakage
    window.multiplyWithWindowingTable(fftPtr, fftSize);
    
    //do the ffts
    if (!bypassed){
        // perform forward fft
        fft.performRealOnlyForwardTransform(fftPtr, true);
        //do stuff with the data
        processSpectrum(fftPtr, numBins, isSunk);
        //perform IFFT
        fft.performRealOnlyInverseTransform(fftPtr);
    }

    //apply the window again for resynthesis
    window.multiplyWithWindowingTable(fftPtr, fftSize);
    
    // scale down the output samples because of the overlapping windows
    for (int i = 0; i < fftSize; ++i){
        fftPtr[i] *= windowCorrection;
    }
   // add the IFFT results ot the output FIFO
    for (int i = 0; i < pos; ++i){
        outputFifo[i] += fftData[i + fftSize - pos];
    }
    for (int i = 0; i < fftSize - pos; ++i){
        outputFifo[i + pos] += fftData[i];
    }
    
}

void Sunk_FFTProcessor::processSpectrum(float* data, int numBins, bool isSunk)
{
    //reinterpret real, imaginary, real, imaginary pattern of floats to complex numbers
    auto* cdata = reinterpret_cast<std::complex<float>*>(data);
    static std::vector<float> initialPhases(numBins);
    static std::vector<float> targetPhases(numBins);
    
    if (isSunk){
        if (needFreeze){
            frozenSpectrum.resize(numBins);
            for (int i = 0; i < numBins; ++i){
                frozenSpectrum[i] = cdata[i];
            }
            needFreeze = false;
        }
        
        //use frozen magnitude, keep current phases
        for (int i = 0; i < numBins; i++){
            float currentPhase = std::arg(cdata[i]);
            float mag = std::abs(frozenSpectrum[i]);
            
            
            cdata[i] = std::polar(mag, currentPhase);
        }
    }
    else{
        needFreeze = true;
        
        
    }
    
    

//    
//    for (int i = 0; i < numBins; ++i){
//        //usually want to work with the magnitude and phanse rather than real and imaginary numbers directly
//        float magnitude = std::abs(cdata[i]);
//        float phase = std::arg(cdata[i]);
//        
//        // this is the spot for spectral processing
//        
//        phase *= float(i);
//
//
//        
//        
//        //convert magnitude and phase back into a complex number
//        cdata[i] = std::polar(magnitude, phase);
//    }
     
}
