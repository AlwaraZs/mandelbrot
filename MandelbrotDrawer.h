#ifndef MANDELBROT_MANDELBROTDRAWER_H
#define MANDELBROT_MANDELBROTDRAWER_H

#include "GuardQImage.h"
#include <functional>
#include <future>
#include <complex>

struct RawImageSpan {
    uchar *data;
    uchar *begin;
    uchar *end;
    long width;
};

class DrawThread {
public:
    std::future<void> draw(QPoint center, RawImageSpan span, int itCount, double scale, int quality, std::complex<double> offset);

    void kill();

    ~DrawThread() {
        kill();
    }

private:
    uint32_t value(int x, int y, int itCount, double scale, std::complex<double> offset) {
        std::complex<double> c(x, y);
        c *= scale;
        c += offset;

        std::complex<double> z = 0;
        size_t i = 0;
        while (std::abs(z) <= 2. && i < itCount) {
            if (killed) {
                break;
            }
            z = z * z + c;
            ++i;
        }

        return (uint32_t)(((double) i) / itCount * std::numeric_limits<uint32_t>::max());
    }

    void drawImpl(QPoint center, RawImageSpan span, int itCount, double scale, int quality, std::complex<double> offset);

    std::thread thread;
    std::atomic<bool> killed{false};
    std::promise<void> promise;
};

class MandelbrotDrawer {
public:
    void paint(GuardQImage::WriteLockedImage writeImage, int itCount, QPoint center, double scale, int quality, std::complex<double> offset);

    void kill();

    static uint32_t value(int x, int y, int itCount, double scale);

    [[nodiscard]] bool ended() const {
        return isEnded;
    }

    ~MandelbrotDrawer() {
        kill();
    }

private:
    std::thread maintainThread;
    std::atomic<bool> isEnded{true};
    std::vector<DrawThread> threads{std::thread::hardware_concurrency()};
};


#endif//MANDELBROT_MANDELBROTDRAWER_H
