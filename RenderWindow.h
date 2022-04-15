#ifndef MANDELBROT_RENDERWINDOW_H
#define MANDELBROT_RENDERWINDOW_H

#include "GuardQImage.h"
#include "MandelbrotDrawer.h"
#include <QMainWindow>
#include <QTimer>

class RenderWindow : public QMainWindow {
    Q_OBJECT
public:
    explicit RenderWindow(QWidget *parent = nullptr) : image(0, 0), QMainWindow(parent) {
        timer.start(10);
        connect(&timer, &QTimer::timeout, this, [this] { this->update(); });
    }

    void paintEvent(QPaintEvent *e) override;

    void mousePressEvent(QMouseEvent *event) override;

    void mouseMoveEvent(QMouseEvent *event) override;

    void wheelEvent(QWheelEvent *event) override;

    static constexpr int startItCount = 100;
    static constexpr int startQuality = 56;
    static constexpr int maxQuality = 1;
private:
    bool updated{false};
    QPoint startMousePos{};
    std::complex<double> shift{};
    int itCount{startItCount};
    int quality{startQuality};
    double scale{0.005};
    QTimer timer;
    GuardQImage image;
    MandelbrotDrawer drawer;
};


#endif//MANDELBROT_RENDERWINDOW_H
