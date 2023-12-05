#include "spectrogram.h"

Spectrogram::Spectrogram(unsigned int _sampleRate,
                         unsigned int _sampleLength,
                         unsigned int _samplesPerLine,
                         unsigned int _numLines)

{
    sampleRate     = _sampleRate;
    sampleLength   = _sampleLength;
    samplesPerLine = _samplesPerLine;
    numLines       = _numLines;
    ringBufferSize = (numLines - 1) * samplesPerLine + sampleLength;
    sampleCounter  = 0;

    waveRingBuffer = new float[ringBufferSize];
    std::fill_n(waveRingBuffer, ringBufferSize, 0.0f);
    ringBufferInd = 0;

    fftSize = 4096;

    headTime  = 0.0f;
    deltaTime = 0.0f;
    deltaTime = ((double)samplesPerLine)/((double)sampleRate);
    frequencyList.clear();
    for (unsigned int indFreq = 0; indFreq < fftSize; indFreq++) {
        float freq = ((float)(indFreq)) * ((float)sampleRate) /((float)fftSize);
        frequencyList.push_back(freq);
    }
}

Spectrogram::~Spectrogram() {
    delete [] waveRingBuffer;
    waveRingBuffer = 0;
}

unsigned int Spectrogram::processData(float *buffer, unsigned int bufferLength) {
    unsigned int newLines = 0;

    float waveEnvMin = 0, waveEnvMax = 0;

    for (unsigned int bufferInd = 0; bufferInd < bufferLength; bufferInd++) {
        float value = buffer[bufferInd];

        if (value > waveEnvMax) {
            waveEnvMax = value;
        }
        if (value < waveEnvMin) {
            waveEnvMin = value;
        }

        waveRingBuffer[ringBufferInd] = value;
        ringBufferInd = (ringBufferInd + 1) % ringBufferSize;
        sampleCounter++;

        if (sampleCounter == fftSize) {
            sampleCounter -= samplesPerLine;

            newLines++;

            float *fftAbs = new float[fftSize];

#ifdef CUDA_FFT
            float *fftData = new float[fftSize];
#else
            std::complex<float> *fftData = new std::complex<float>[fftSize];
#endif
            unsigned int startIndex = (ringBufferInd - fftSize + ringBufferSize) % ringBufferSize;

            for (unsigned int indBuffer = 0; indBuffer < fftSize; indBuffer++) {
                unsigned int sampleIndex = (startIndex + indBuffer) % ringBufferSize;
                fftData[indBuffer] = waveRingBuffer[sampleIndex];
                //std::cout << sampleIndex << " " << indBuffer << " " << fftData[indBuffer] << std::endl;
            }

#ifdef CUDA_FFT
            PerformCUDAFFT(fftData, fftData, fftSize);
#else
            FFTCompute(fftData, fftSize);
#endif
            for (unsigned int indBuffer = 0; indBuffer < fftSize; indBuffer++) {
                fftAbs[indBuffer] = std::abs(fftData[indBuffer]) / ((float)fftSize);
            }
            // Store the new line in the spectrogram:
            addLine(fftAbs, fftSize, waveEnvMin, waveEnvMax);

            delete [] fftData;
            delete [] fftAbs;
        }
    }

    return newLines;
}
