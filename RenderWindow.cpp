#include "RenderWindow.h"
#include <QMouseEvent>
#include <QPainter>

void RenderWindow::paintEvent(QPaintEvent *e) {
    QPainter painter{this};
    painter.drawImage(0, 0, image.lockForRead().read());
    if (image.height() != height() || image.width() != width() || updated) {
        updated = false;
        drawer.kill();
        itCount = startItCount;
        quality = startQuality;
    } else if (quality < maxQuality) {
        return;
    }

    if (drawer.ended()) {
        drawer.kill();
        QPoint center = QPoint(width() / 2, height() / 2);
        image.resize(width(), height());
        drawer.paint(
                image.lockForWrite(),
                itCount, center, scale, quality, shift);
        quality -= 5;
    }
}

void RenderWindow::mousePressEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton) {
        startMousePos = event->pos();
    }
}
void RenderWindow::mouseMoveEvent(QMouseEvent *event) {
    if (event->buttons() & Qt::LeftButton) {
        auto point = (event->pos() - startMousePos);
        shift -= std::complex{point.x() * scale, point.y() * scale};
        startMousePos = event->pos();
        updated = true;
    }
}
void RenderWindow::wheelEvent(QWheelEvent *event) {
    const double numDegrees = event->angleDelta().y() / 8.;
    const double numSteps = numDegrees / 15.;
    scale *= pow(0.8, numSteps);
    updated = true;
}
