#pragma once
#include <QImage>
#include <SDL3/SDL.h>

// Qt RGB32 requires 0xffRRGGBB. SDL XRGB surfaces do not guarantee that
// unused byte, so wrapping their pixels directly can make Qt paint holes.
inline QImage ImageFromSDLSurface(const SDL_Surface& surface) {
    QImage image(surface.w, surface.h, QImage::Format_RGB32);
    if (image.isNull() || !SDL_ConvertPixels(surface.w, surface.h,
            surface.format, surface.pixels, surface.pitch,
            SDL_PIXELFORMAT_ARGB8888, image.bits(), image.bytesPerLine()))
        return QImage();
    for (int y = 0; y < image.height(); ++y) {
        auto pixels = reinterpret_cast<QRgb*>(image.scanLine(y));
        for (int x = 0; x < image.width(); ++x) pixels[x] |= 0xff000000u;
    }
    return image;
}
