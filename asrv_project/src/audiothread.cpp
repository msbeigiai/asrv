#include "audiothread.h"

#include <QMediaDevices>
#include <QAudioDevice>
#include <QAudioSink>
#include <QCoreApplication>

#define PA_SAMPLE_FLOAT32 PA_SAMPLE_FLOAT32

AudioThread::AudioThread(unsigned int _sampleRate,
                         unsigned int _bufferSize) {

    sampleRate = _sampleRate;
    bufferSize = _bufferSize;

    bufferLeft = new float[bufferSize];
    bufferRight = new float[bufferSize];
    copyBufferLeft = new float[bufferSize];
    copyBufferRight = new float[bufferSize];

    stopped = true;

    bufferIndex = 0;

    QAudioFormat formatAudio;
    formatAudio.setSampleRate(sampleRate);
    formatAudio.setChannelCount(1);
    formatAudio.setSampleFormat(QAudioFormat::Float);

    QAudioDevice info(QMediaDevices::defaultAudioInput());

    m_audioInput = new QAudioInput(info, this);

    m_audioSource = new QAudioSource(info, formatAudio);
    m_audioSource->setBufferSize(RAW_BUFFERSIZE);

    m_device = m_audioSource->start();
    m_device->open(QIODevice::WriteOnly);
}

AudioThread::~AudioThread(){
    delete [] bufferLeft;
    delete [] bufferRight;
    delete [] copyBufferLeft;
    delete [] copyBufferRight;
    bufferLeft = 0;
    bufferRight = 0;
    copyBufferLeft = 0;
    copyBufferRight = 0;

    delete m_audioInput;
    delete m_device;
}

void AudioThread::run() {
    stopped = false;
    int error;
    float rawBuffer[RAW_BUFFERSIZE];

    while(!stopped) {
        //m_device = m_audioSource->start();

        if(m_device->waitForReadyRead(-1)) {
            qint64 numRead = m_device->read((char*)rawBuffer, RAW_BUFFERSIZE * sizeof(float));
            if (numRead < 0) {
                qErrnoWarning(error, "Error reading audio data");
                QCoreApplication::quit();
            }

            if (numRead % 2 != 0) {
                qWarning("Non-even number of samples read!");
            } else {
                for (int indRead = 0; indRead < RAW_BUFFERSIZE / 8; indRead++) {
                    bufferLeft[bufferIndex] = rawBuffer[indRead*2];
                    bufferRight[bufferIndex] = rawBuffer[indRead*2 + 1];

                    bufferIndex++;

                    if (bufferIndex == bufferSize) {

                        for (int unsigned ind = 0; ind < bufferSize; ind++) {
                            copyBufferLeft[ind] = bufferLeft[ind];
                            copyBufferRight[ind] = bufferRight[ind];
                        }

                        emit bufferFilled(copyBufferLeft,  bufferSize);
                        bufferIndex = 0;
                    }
                }
            }
        }
        m_audioSource->stop();
    }
}

void AudioThread::stop() {
    if(m_device->isOpen()) {
        m_device->close();
    }
    stopped = true;
}
