#include "spectrogramwidget.h"
#include <QStylePainter>

SpectrogramWidget::SpectrogramWidget(Spectrogram *_spectrogram,
                                     QWidget *parent,
                                     double _minFreq,
                                     double _maxFreq,
                                     double _minAmpl,
                                     double _maxAmpl)
    : QWidget{parent} {

    spectrogram = _spectrogram;
    minFreq = _minFreq;
    maxFreq = _maxFreq;
    minAmpl = _minAmpl;
    maxAmpl = _maxAmpl;

    paddingX = 20;
    paddingY = 20;
    xlabelSpacing = 40;
    ylabelSpacing = 80;

    freqTickBig   = 10000.0;
    freqTickSmall = 1000.0;

    drawMode = DRAWMODE_SCROLL;
    layoutMode = LAYOUT_HORIZONTAL;
    drawTimeGrid = true;
    drawFreqGrid = true;
    drawColorbar = true;
    drawSpectrum = true;
    drawWaveform = true;

    waveformWidth = 100;
    spectrumWidth = 100;
    colorBarWidth = 50;

    backgroundColor = QColor(0, 0, 0);
    gridColor = QColor(128, 128, 128);

    timeScroll = 0.0;
    image = 0;

    logScaleFreq = true;
    logScaleAmpl = true;

    QVector<float> color0, color1, color2, color3, color4;

    color0.push_back(0.0);  color0.push_back(0.0);  color0.push_back(32.0);color0.push_back(0.0);
    color1.push_back(0.0);color1.push_back(0.0);  color1.push_back(255.0);color1.push_back(0.25);
    color2.push_back(0.0);color2.push_back(255.0);color2.push_back(255.0);color2.push_back(0.5);
    color3.push_back(255.0);color3.push_back(255.0);color3.push_back(0.0);color3.push_back(0.75);
    color4.push_back(255.0);color4.push_back(0.0);color4.push_back(0.0);color4.push_back(1.0000);
    colormap.push_back(color0);
    colormap.push_back(color1);
    colormap.push_back(color2);
    colormap.push_back(color3);
    colormap.push_back(color4);
}

void SpectrogramWidget::evalColormap(float value, int &r, int &g, int &b) {
    int nRGB = colormap.size();

    QVector<float> RGB;
    QVector<float> RGBnext;

    for (int indRGB = 0; indRGB < nRGB-1; indRGB++) {
        RGB = colormap[indRGB];
        RGBnext = colormap[indRGB+1];

        if (value < RGB[3]) {
            r = (int)RGB[0];
            g = (int)RGB[1];
            b = (int)RGB[2];
            return;
        } else if (value <= RGBnext[3]) {
            float valcoeff = (value - RGB[3]) / (RGBnext[3] - RGB[3]);

            r = (int)(RGB[0] * (1.0 - valcoeff) + valcoeff * RGBnext[0]);
            g = (int)(RGB[1] * (1.0 - valcoeff) + valcoeff * RGBnext[1]);
            b = (int)(RGB[2] * (1.0 - valcoeff) + valcoeff * RGBnext[2]);
            return;
        }
    }
    r = (int)RGBnext[0];
    g = (int)RGBnext[1];
    b = (int)RGBnext[2];
}

void SpectrogramWidget::paintEvent(QPaintEvent *event) {
    QStylePainter painter(this);
    painter.drawPixmap(0, 0, pixmap);
    Q_UNUSED(event);
}

void SpectrogramWidget::resizeEvent(QResizeEvent *event) {
    plotwidth  = width()  - paddingX * 2 - ylabelSpacing;
    plotheight = height() - paddingY * 2 - xlabelSpacing;
    plotx = paddingX + ylabelSpacing;
    ploty = paddingY;

    if (layoutMode == LAYOUT_HORIZONTAL) {
        if (drawSpectrum) {
            plotwidth -= spectrumWidth;
            plotwidth -= colorBarWidth;
        }
        if (drawWaveform) {
            plotheight -= waveformWidth;
            ploty += waveformWidth;
        }
    }
    if (layoutMode == LAYOUT_VERTICAL) {
        if (drawSpectrum) {
            ploty += spectrumWidth;
            plotheight -= spectrumWidth;
            plotheight -= colorBarWidth;
        }
        if (drawWaveform) {
            plotwidth -= waveformWidth;
        }
    }

    renderImage(0, true);
    refreshPixmap();
    Q_UNUSED(event);
}

int SpectrogramWidget::freqToPixel(double freq) {
    double minCoord;
    double imageSize;

    if (layoutMode == LAYOUT_HORIZONTAL) {
        minCoord  = (double)ploty;
        imageSize = (double)plotheight;
    } else if (layoutMode == LAYOUT_VERTICAL) {
        minCoord  = (double)plotx;
        imageSize = (double)plotwidth;
    } else {
        qWarning("freqToPixel : Non-supported Layout Mode %d!", layoutMode);
        return 0;
    }

    if (logScaleFreq) {

        double b = imageSize / (log10(minFreq) - log10(maxFreq));
        double a = minCoord - b * log10(maxFreq);

        if (layoutMode == LAYOUT_HORIZONTAL) {
            return (int)(a + b * log10(freq));
        } else if (layoutMode == LAYOUT_VERTICAL) {
            return (int)(2 * minCoord + imageSize - a - b * log10(freq));
        }
    } else {

        double b = imageSize / (minFreq - maxFreq);
        double a = minCoord - maxFreq * b;
        if (layoutMode == LAYOUT_HORIZONTAL) {
            return (int)(a + b * freq);
        } else if (layoutMode == LAYOUT_VERTICAL) {
            return (int)(2 * minCoord + imageSize - a - b * freq);
        }
    }
    return 0;
}

int SpectrogramWidget::timeToPixel(double time) {
    double deltaTime = spectrogram->getDeltaTime();
    double headTime  = spectrogram->getHeadTime();
    double timeWidth = ((double)plotwidth) * deltaTime;

    if (layoutMode == LAYOUT_HORIZONTAL) {
        double b = plotwidth / timeWidth;
        double a = plotx + plotwidth - b * headTime;

        double x_double = a  + b * (time - timeScroll);
        return (int) x_double;
    } else if (layoutMode == LAYOUT_VERTICAL) {
        double b = plotheight / timeWidth;
        double a = ploty + plotheight - b * headTime;

        double y_double = a  + b * (time - timeScroll);
        return (int) y_double;
    }
    return 0;
}

void SpectrogramWidget::drawGrid(QPainter &painter) {
    double deltaTime = spectrogram->getDeltaTime();
    double headTime  = spectrogram->getHeadTime();
    double timeWidth = (double)(plotwidth) * deltaTime;

    //qDebug("deltaTime %f, headTime %f, timeWidth %f", deltaTime, headTime, timeWidth);

    QPen thickPen, thinPen, dashPen;
    thinPen.setStyle(Qt::SolidLine);
    thinPen.setWidth(1);
    thinPen.setColor(gridColor);
    dashPen.setStyle(Qt::DotLine);
    dashPen.setWidth(1);
    dashPen.setColor(gridColor);

    painter.setPen(dashPen);
    painter.drawRect(plotx, ploty, plotwidth, plotheight);
    for (unsigned int indSec = 1; indSec <= (unsigned int)(10.0*timeWidth); indSec++) {

        if (layoutMode == LAYOUT_HORIZONTAL) {
            int x = timeToPixel(headTime - 0.1 * (double)indSec);
            if (x > (int)plotx && x < (int)(plotwidth + plotx)) {
                painter.drawLine(x, ploty, x, ploty + plotheight);
            }
        } else if (layoutMode == LAYOUT_VERTICAL) {
            int y = timeToPixel(headTime - 0.1 * (double)indSec);
            if (y > (int)ploty && y < (int)(plotheight + ploty)) {
                painter.drawLine(plotx, y, plotx + plotwidth, y);
            }
        }
    }
    painter.setPen(thinPen);
    for (unsigned int indSec = 0; indSec <= (unsigned int)timeWidth; indSec++) {
        if (layoutMode == LAYOUT_HORIZONTAL) {
            int x = timeToPixel(headTime - (double)indSec);
            if (x >= (int)plotx && x <= (int)(plotwidth + plotx)) {
                painter.drawLine(x, ploty, x, ploty + plotheight + 10);
            }
        } else if (layoutMode == LAYOUT_VERTICAL) {
            int y = timeToPixel(headTime - (double)indSec);
            if (y >= (int)ploty && y <= (int)(plotheight + ploty)) {
                painter.drawLine(plotx-10, y, plotx + plotwidth, y);
            }
        }
    }

    if (logScaleFreq) {
        for (int lfreq = (int) log10(minFreq); lfreq <= (int)log10(maxFreq); lfreq++) {
            double freq = pow(10.0, lfreq);

            painter.setPen(dashPen);
            for (int fmult = 1; fmult < 10; fmult++) {
                if (layoutMode == LAYOUT_HORIZONTAL) {
                    int y = freqToPixel(freq*(double)fmult);
                    if (y >= (int)ploty && y <= (int)(ploty + plotheight)) {
                        painter.drawLine(plotx, y, plotx + plotwidth, y);
                    }
                } else if (layoutMode == LAYOUT_VERTICAL) {
                    int x = freqToPixel(freq*(double)fmult);
                    if (x >= (int)plotx && x <= (int)(plotx + plotwidth)) {
                        painter.drawLine(x, ploty, x, ploty + plotheight);
                    }
                }
            }

            painter.setPen(thinPen);
            if (layoutMode == LAYOUT_HORIZONTAL) {
                int y = freqToPixel(freq);
                painter.drawText(QRect(paddingX-15 , y-10, ylabelSpacing, 20), Qt::AlignRight | Qt::AlignVCenter,
                                 QString::number(freq) + QString(" Hz"));
                if (y >= (int)ploty && y <= (int)(ploty + plotheight)) {
                    painter.drawLine(plotx-10, y, plotx + plotwidth, y);
                }
            } else if (layoutMode == LAYOUT_VERTICAL) {
                int x = freqToPixel(freq);
                painter.drawText(QRect(x-30, ploty + plotheight + 15, 60, 20), Qt::AlignHCenter,
                                 QString::number(freq) + QString(" Hz"));
                if (x >= (int)plotx && x <= (int)(plotx + plotwidth)) {
                    painter.drawLine(x, ploty, x, ploty + plotheight + 10);
                }
            }
        }
    } else {
        for (double freq = floor(minFreq/freqTickSmall); freq <= floor(maxFreq/freqTickSmall); freq += 1) {
            painter.setPen(dashPen);
            if (layoutMode == LAYOUT_HORIZONTAL) {
                int y = freqToPixel(freq * freqTickSmall);
                if (y >= (int)ploty && y <= (int)(ploty + plotheight)) {
                    painter.drawLine(plotx-10, y, plotx + plotwidth, y);
                    painter.drawText(QRect(paddingX-15 , y-10, ylabelSpacing, 20), Qt::AlignRight | Qt::AlignVCenter,
                                     QString::number(freq * freqTickSmall) + QString(" Hz"));
                }
            } else if (layoutMode == LAYOUT_VERTICAL) {
                int x = freqToPixel(freq * freqTickSmall);
                if (x >= (int)plotx && x <= (int)(plotx + plotwidth)) {
                    painter.drawLine(x, ploty, x, ploty + plotheight);
                }

            }
        }
        for (double freq = floor(minFreq/freqTickBig); freq <= floor(maxFreq/freqTickBig); freq += 1) {
            painter.setPen(thinPen);
            if (layoutMode == LAYOUT_HORIZONTAL) {
                int y = freqToPixel(freq * freqTickBig);
                if (y >= (int)ploty && y <= (int)(ploty + plotheight)) {
                    painter.drawLine(plotx-10, y, plotx + plotwidth, y);
                }
            } else if (layoutMode == LAYOUT_VERTICAL) {
                int x = freqToPixel(freq * freqTickBig);
                if (x >= (int)plotx && x <= (int)(plotx + plotwidth)) {
                    painter.drawLine(x, ploty, x, ploty + plotheight + 10);
                    painter.drawText(QRect(x, ploty + plotheight + 10, 80, 20), Qt::AlignHCenter,
                                     QString::number(freq * freqTickBig) + QString(" Hz"));
                }
            }
        }
    }
}
