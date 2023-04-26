#include "texture.hpp"
#include <cstring>

static void reset(Texture& t);

Texture::Texture(const Texture& t) : size(t.size), width(t.width), height(t.height)
    , bytesPerPixel(t.bytesPerPixel), format(t.format) {

    pixels = new byte[size];
    memcpy(pixels, t.pixels, size); 
}

Texture::Texture(Texture&& t) : size(t.size), width(t.width), height(t.height)
    , bytesPerPixel(t.bytesPerPixel), format(t.format), pixels(t.pixels) {

    reset(t);
}

Texture& Texture::operator=(const Texture& t) {
    size = t.size;
    width = t.width;
    height = t.height;
    bytesPerPixel = t.bytesPerPixel;
    format = t.format;
    pixels = new byte[size];
    memcpy(pixels, t.pixels, size);
    return *this;
}

Texture& Texture::operator=(Texture&& t) {
    size = t.size;
    width = t.width;
    height = t.height;
    bytesPerPixel = t.bytesPerPixel;
    format = t.format;
    pixels = t.pixels;
    reset(t);    
    return *this;
}

Texture::~Texture() {
    delete[] pixels;
}

static void reset(Texture& t) {
    t.size = 0;
    t.width = 0;
    t.height = 0;
    t.bytesPerPixel = 0;
    t.format = VK_FORMAT_UNDEFINED;
    t.pixels = nullptr;
}
