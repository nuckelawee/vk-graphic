#pragma once

class BinaryIfstream;

enum TgaFormat { 
    TGA_FORMAT_NO_IMAGE = 0,
    TGA_FORMAT_IMAGE_WITH_PALETTE = 1,
    TGA_FORMAT_TRUE_COLOR_IMAGE = 2,
    TGA_FORMAT_MONOCHROME_IMAGE = 3,
    TGA_FORMAT_RLE_IMAGE_WITH_PALETTE = 9,
    TGA_FORMAT_RLE_TRUE_COLOR_IMAGE = 10,
    TGA_FORMAT_RLE_MONOCHROME_IMAGE = 11,
};

struct TgaColorMapInfo {
    unsigned char *data;
    uint16_t firstIndex;
    uint16_t length;
    unsigned char type; 
    unsigned char bitsPerElement;
};

struct Tga {
    TgaColorMapInfo colorMap;
    uint32_t imageBytesSize;
    uint16_t width;
    uint16_t height;
    char dataType;
    char bitsPerPixel; 
    char description;
    bool fakeAlpha;
};

void flipTgaImage(Texture& texture);

void loadInfoAboutTga(Tga& tga, Texture& texture
     , const BinaryIfstream& file, bool supportAlpha);

void loadColorMapTga(Tga& tga, Texture& texture, unsigned char *head
    , const BinaryIfstream& file);

void loadRleTrueColorImageTga(Tga& tga, Texture& texture
    , const BinaryIfstream& file);
    
void loadTrueColorImageTga(Tga& tga, Texture& texture
    , const BinaryIfstream& file);
    
void loadColorMapImageTga(Tga& tga, Texture& texture
    , const BinaryIfstream& file);

void loadRleColorMapImageTga(Tga& tga, Texture& texture
    , const BinaryIfstream& file);

void identifierTga(unsigned char idLength, const BinaryIfstream& file);

void allocateImageMemoryTga(Tga& tga, Texture& texture
    , const BinaryIfstream& file);

