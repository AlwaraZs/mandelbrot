#ifndef MANDELBROT_GUARDQIMAGE_H
#define MANDELBROT_GUARDQIMAGE_H

#include <QImage>
#include <QReadWriteLock>

class GuardQImage {
public:
    GuardQImage() = default;
    GuardQImage(int width, int height) : image(width, height, format), lock{} {}

    class ReadLockedImage {
    public:
        ReadLockedImage(const QImage &image, QReadWriteLock &lock) : image(image), lock(lock) {
            lock.lockForRead();
        }

        const QImage &read() {
            return image;
        }

        ~ReadLockedImage() {
            lock.unlock();
        }

    private:
        const QImage &image;
        QReadWriteLock &lock;
    };

    class WriteLockedImage {
    public:
        WriteLockedImage(QImage &image, QReadWriteLock &lock) : image(image), lock(lock) {
            lock.lockForWrite();
        }

        QImage &write() {
            return image;
        }

        void unlock() {
            lock.unlock();
            locked = false;
        }

        ~WriteLockedImage() {
            if (locked) {
                lock.unlock();
            }
        }

    private:
        bool locked = true;
        QImage &image;
        QReadWriteLock &lock;
    };

    [[nodiscard]] ReadLockedImage lockForRead() {
        return ReadLockedImage(image, lock);
    }

    [[nodiscard]] WriteLockedImage lockForWrite() {
        return WriteLockedImage(image, lock);
    }

    void resize(int width, int height) {
        if (width != image.width() || height != image.height()) {
            image = QImage(width, height, format);
        }
    }

    [[nodiscard]] int width() const {
        return image.width();
    }

    [[nodiscard]] int height() const {
        return image.height();
    }

    constexpr static auto format = QImage::Format_RGB32;
    constexpr static auto pixelSize = 4;

private:
    QImage image;
    QReadWriteLock lock;
};


#endif//MANDELBROT_GUARDQIMAGE_H
