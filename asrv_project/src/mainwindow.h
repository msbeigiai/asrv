#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "audiothread.h"
#include "spectrogram.h"
#include "spectrogramwidget.h"

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    AudioThread* m_audioThread = nullptr;

protected:
    void keyPressEvent(QKeyEvent *event);

private:
    Spectrogram* m_spectrogram = nullptr;
    SpectrogramWidget* m_spectrogramWidget = nullptr;

signals:
};

#endif // MAINWINDOW_H
