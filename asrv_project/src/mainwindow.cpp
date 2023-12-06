#include "mainwindow.h"

#include <QSplineSeries>
#include <QChart>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    m_spectrogram = new Spectrogram(44100, 44100 * 60, 256, 8192);
    m_spectrogramWidget = new SpectrogramWidget(m_spectrogram, this);

    setCentralWidget(m_spectrogramWidget);

    resize(1024, 600);

    m_audioThread = new AudioThread(44100, 1024);
    m_audioThread->start();

    // TODO
    connect(m_audioThread, SIGNAL(bufferFilled(float*,uint)),
            m_spectrogramWidget, SLOT(processData(float*,uint)));
}

MainWindow::~MainWindow() {
    delete m_spectrogram;
}

void MainWindow::keyPressEvent(QKeyEvent* event) {
    qDebug("keyPress %d", event->key());

    switch (event->key()) {
    case Qt::Key_F1:
        m_spectrogramWidget->setMinAmpl(0.0001);
        break;
    case Qt::Key_F2:
        m_spectrogramWidget->setMinAmpl(0.001);
        break;
    case Qt::Key_F3:
        m_spectrogramWidget->setMinAmpl(0.01);
        break;
    case Qt::Key_F4:
        m_spectrogramWidget->setMaxAmpl(0.1);
        break;
    case Qt::Key_F5:
        m_spectrogramWidget->setMaxAmpl(1.0);
        break;
    case Qt::Key_F6:
        m_spectrogramWidget->setMaxAmpl(10.0);
        break;
    }
}
