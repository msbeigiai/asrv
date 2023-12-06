#include "audiothread.h"

#define PA_SAMPLE_FLOAT32 PA_SAMPLE_FLOAT32

AudioThread::AudioThread(unsigned int _sampleRate,
                         unsigned int _bufferSize) {

    sampleRate = _sampleRate;
    bufferSize = _bufferSize;
}
