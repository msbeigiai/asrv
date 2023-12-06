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

    void stop();

    unsigned int sampleRate;
    unsigned int bufferSize;

signals:
    void bufferFilled(float *outputBufferLeft,
                      unsigned int bufferLength);

protected:
    void run() override;

private:
    volatile bool stopped;
    float *bufferLeft, *bufferRight;
    float *copyBufferLeft, *copyBufferRight;

    unsigned int bufferIndex;


    QAudioInput *m_audioInput = nullptr;
    QAudioSource* m_audioSource = nullptr;
    QIODevice* m_device = nullptr;


private:

};

#endif // AUDIOTHREAD_H
