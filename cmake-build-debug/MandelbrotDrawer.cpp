#include "MandelbrotDrawer.h"
#include "RenderWindow.h"
#include <cmath>
#include <complex>
#include <cstring>
#include <future>


void MandelbrotDrawer::paint(GuardQImage::WriteLockedImage writeImage, int itCount, QPoint center, double scale, int quality, std::complex<double> offset) {
    isEnded = false;
    maintainThread = std::thread([writeImage, this, itCount, center, scale, quality, offset]() mutable {
        QImage &image = writeImage.write();

        long sliceSize = (image.width() * image.height()) / threads.size();

        uchar *raw = image.bits();

        std::vector<std::future<void>> futures;
        futures.reserve(threads.size());

        for (int i = 0; i < threads.size() - 1; ++i) {
            futures.push_back(threads[i].draw(center, RawImageSpan{image.bits(), raw, raw + sliceSize * GuardQImage::pixelSize, image.width()},
                                              itCount, scale, quality, offset));
            raw += sliceSize * GuardQImage::pixelSize;
        }

        futures.push_back(threads.back().draw(center, RawImageSpan{image.bits(), raw, image.bits() + image.width() * image.height() * GuardQImage::pixelSize, image.width()},
                                              itCount, scale, quality, offset));

        for (auto &f : futures) {
            f.get();
        }
        isEnded = true;
    });
}

void MandelbrotDrawer::kill() {
    for (auto &t : threads) {
        t.kill();
    }

    if (maintainThread.joinable()) {
        maintainThread.join();
    }
}

uint32_t MandelbrotDrawer::value(int x, int y, int itCount, double scale) {
    auto const sv = itCount;
    auto p = std::sqrt(std::pow(x - 0.25, 2) + y * y);
    auto theta = std::atan2(y, x - 0.25);
    auto pc = 0.5 - 0.5 * std::cos(theta);

    if (p <= pc) {
        return 0;// black
    }

    std::complex<double> c(x, y);
    c *= scale;

    std::complex<double> z = 0;
    size_t i = 0;
    while (std::abs(z) <= 2. && i < itCount) {
        z = z * z + c;
        ++i;
    }

    return (uint32_t)(((double) i) / itCount * std::numeric_limits<uint32_t>::max());
}

std::future<void> DrawThread::draw(QPoint center, RawImageSpan span, int itCount, double scale, int quality, std::complex<double> offset) {
    kill();
    promise = std::promise<void>{};
    thread = std::thread([this, center, span, itCount, scale, quality, offset]() {
        drawImpl(center, span, itCount, scale, quality, offset);
        promise.set_value();
    });
    return promise.get_future();
}

void DrawThread::kill() {
    killed = true;
    if (thread.joinable()) {
        thread.join();
    }
    killed = false;
}

void DrawThread::drawImpl(QPoint center, RawImageSpan span, int itCount, double scale, int quality, std::complex<double> offset) {
    uint32_t pixVal{};
    for (auto i = span.begin; i < span.end; i += GuardQImage::pixelSize) {
        if (killed) {
            return;
        }
        size_t x = ((i - span.data) / GuardQImage::pixelSize) % span.width;
        size_t y = (i - span.data) / GuardQImage::pixelSize / span.width;
        if ((i - span.begin) % (quality * GuardQImage::pixelSize) == 0) {
            pixVal = value(x - center.x(), y - center.y(), itCount, scale, offset);
        }
        std::memcpy(i, &pixVal, GuardQImage::pixelSize);
    }
}
