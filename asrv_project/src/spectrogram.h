#ifndef SPECTROGRAM_H
#define SPECTROGRAM_H

#include <QThread>

class Spectrogram : public QThread
{
public:
    explicit Spectrogram(QObject *parent = nullptr);
};

#endif // SPECTROGRAM_H
