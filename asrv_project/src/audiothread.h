#ifndef AUDIOTHREAD_H
#define AUDIOTHREAD_H

#include <QAudioFormat>
#include <QAudioInput>
#include <QAudioSource>
#include <QThread>

#define RAW_BUFFERSIZE 128

class AudioThread : public QThread
{
    Q_OBJECT
public:
    explicit AudioThread(unsigned int _sampleRate,
                         unsigned int _bufferSize);

    ~AudioThread();

    unsigned int sampleRate;
    unsigned int bufferSize;
};

#endif // AUDIOTHREAD_H
