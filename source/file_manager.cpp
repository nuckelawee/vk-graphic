#include "file_manager.hpp"
#include "tga.hpp"
#include "binary_ifstream.hpp"
#include "texture.hpp"

std::string FileManager::ReadFile(const std::string& filepath
    , std::ios_base::openmode mode) {

    std::ifstream file(filepath, mode);
    if(file.is_open() == false) {
        throw std::invalid_argument("Failed to open file [ " + filepath + " ]");
    }
    size_t fileSize = static_cast<size_t>(file.tellg());
    std::string code;
    code.resize(fileSize);
    file.seekg(0);
    file.read(code.data(), fileSize);

    file.close();
    return code;
}

Texture FileManager::ReadImageTga(const std::string& filepath
    , bool supportAlpha) {

    BinaryIfstream file(filepath);
    
    Tga tga;
    Texture texture;
    loadInfoAboutTga(tga, texture, file, supportAlpha);

    switch(tga.dataType) {
    case TGA_FORMAT_NO_IMAGE:
        throw std::invalid_argument("File [ " + filepath + " ] doesn't contain image");
    case TGA_FORMAT_IMAGE_WITH_PALETTE:
        loadColorMapImageTga(tga, texture, file);
        break;
    case TGA_FORMAT_TRUE_COLOR_IMAGE:
        loadTrueColorImageTga(tga, texture, file);
        break;
    case TGA_FORMAT_RLE_IMAGE_WITH_PALETTE:
        loadRleColorMapImageTga(tga, texture, file);
        break;
    case TGA_FORMAT_RLE_TRUE_COLOR_IMAGE:
        loadRleTrueColorImageTga(tga, texture, file);
        break;
    case TGA_FORMAT_MONOCHROME_IMAGE:
    case TGA_FORMAT_RLE_MONOCHROME_IMAGE:
        throw std::invalid_argument("This type of tga doesn't support [ " + filepath + " ]");
    default:
        throw std::invalid_argument("This file [ " + filepath + " ] is not tga");
    }
    if((tga.description & (1 << 5)) == 0) {
        flipTgaImage(texture);
    }
    return texture;
}

void flipTgaImage(Texture& texture) {
    uint32_t rowSize = texture.width * texture.bytesPerPixel;
    unsigned char row[rowSize];
    for(size_t i = 0; i < texture.height / 2; i++) {
        memcpy(row, texture.pixels + (i * rowSize), rowSize);
        memcpy(texture.pixels + (i * rowSize)
            , texture.pixels + ((texture.height - i - 1) * rowSize), rowSize);
        memcpy(texture.pixels + ((texture.height - i - 1) * rowSize), row, rowSize);
    }
}

void loadColorMapImageTga(Tga& tga, Texture& texture
    , const BinaryIfstream& file) {

    allocateImageMemoryTga(tga, texture, file);

    uint32_t pixelCount = tga.width * tga.height;
    uint32_t currentByte = 0;
    unsigned char bytesPerElement = tga.colorMap.bitsPerElement / 8;
    unsigned char index;
    
    for(size_t currentPixel = 0; currentPixel < pixelCount; currentPixel++) {
        file.Fread(&index, sizeof(unsigned char), 1);
        texture.pixels[currentByte] = tga.colorMap.data[index * bytesPerElement + 2];
        texture.pixels[currentByte+1] = tga.colorMap.data[index * bytesPerElement + 1];
        texture.pixels[currentByte+2] = tga.colorMap.data[index * bytesPerElement];
        if(tga.fakeAlpha) {
            texture.pixels[currentByte+3] = 255;
        } else if(texture.format == VK_FORMAT_R8G8B8A8_SRGB) {
            texture.pixels[currentByte+3] = tga.colorMap.data
                [index * bytesPerElement + 3];
        }
        currentByte += texture.bytesPerPixel;
    }
}

void loadRleColorMapImageTga(Tga& tga, Texture& texture
    , const BinaryIfstream& file) {

    allocateImageMemoryTga(tga, texture, file);

    uint32_t pixelCount = tga.width * tga.height;
    uint32_t currentPixel = 0;
    uint32_t currentByte = 0;
    unsigned char bytesPerElement = tga.colorMap.bitsPerElement / 8;
    unsigned char index;
    unsigned char chunk;

    while(currentPixel < pixelCount) {
        file.Fread(&chunk, sizeof(unsigned char), 1);
        char isCompressed = chunk & 128;
        unsigned char chunkPixels = chunk & 127;
        if(isCompressed) {
            file.Fread(&index, sizeof(unsigned char), 1);
            for(size_t i = 0; i <= chunkPixels; i++) {
                texture.pixels[currentByte] = tga.colorMap.data[index * bytesPerElement+2];
                texture.pixels[currentByte+1] = tga.colorMap.data[index * bytesPerElement+1];
                texture.pixels[currentByte+2] = tga.colorMap.data[index * bytesPerElement];
                if(tga.fakeAlpha) {
                    texture.pixels[currentByte+3] = 255;
                } else if(texture.format == VK_FORMAT_R8G8B8A8_SRGB) {
                    texture.pixels[currentByte+3]
                        = tga.colorMap.data[index * bytesPerElement+3];
                }
                currentByte += texture.bytesPerPixel;
                currentPixel++;
            }
       } else {
            for(size_t i = 0; i <= chunkPixels; i++) {
                file.Fread(&index, sizeof(unsigned char), 1);
                texture.pixels[currentByte] = tga.colorMap.data[index * bytesPerElement+2];
                texture.pixels[currentByte+1] = tga.colorMap.data[index * bytesPerElement+1];
                texture.pixels[currentByte+2] = tga.colorMap.data[index * bytesPerElement];
                if(tga.fakeAlpha) {
                    texture.pixels[currentByte+3] = 255;
                } else if(texture.format == VK_FORMAT_R8G8B8A8_SRGB) {
                    texture.pixels[currentByte+3] = tga.colorMap.data
                        [index * bytesPerElement + 3];
                }
                currentByte += texture.bytesPerPixel;
                currentPixel++;
            }
        }        
    }
}

void loadTrueColorImageTga(Tga& tga, Texture& texture
    , const BinaryIfstream& file) {

    allocateImageMemoryTga(tga, texture, file);

    if(tga.fakeAlpha) {
        unsigned char colorBuffer[3];
        uint32_t pixelCount = tga.width * tga.height;
        uint32_t currentByte = 0;
        for(size_t currentPixel = 0; currentPixel < pixelCount; currentPixel++) {
            file.Fread(colorBuffer, 1, 3);
            texture.pixels[currentByte] = colorBuffer[2]; 
            texture.pixels[currentByte+1] = colorBuffer[1]; 
            texture.pixels[currentByte+2] = colorBuffer[0]; 
            texture.pixels[currentByte+3] = 255;
            currentByte += texture.bytesPerPixel;
        }
    } else {
        file.Fread(texture.pixels, 1, tga.imageBytesSize);
    } 
}

void loadRleTrueColorImageTga(Tga& tga, Texture& texture
    , const BinaryIfstream& file) {

    allocateImageMemoryTga(tga, texture, file);

    uint32_t pixelCount = tga.width * tga.height;
    uint32_t currentPixel = 0;
    uint32_t currentByte = 0;
    uint32_t colorBufferSize = texture.bytesPerPixel;
    if(tga.fakeAlpha) {
        colorBufferSize = 3;
    }
    unsigned char colorBuffer[colorBufferSize]; 

    while (currentPixel < pixelCount) {
        unsigned char chunkHeader = 0;
        file.Fread(&chunkHeader, sizeof(unsigned char), 1);

        char isCompressed = chunkHeader & 128;
        unsigned char chunkPixels = chunkHeader & 127;
        if(isCompressed) {
            file.Fread(colorBuffer, 1, colorBufferSize);
            for(size_t i = 0; i <= chunkPixels; i++) {
                texture.pixels[currentByte] = colorBuffer[2];
                texture.pixels[currentByte+1] = colorBuffer[1];
                texture.pixels[currentByte+2] = colorBuffer[0];
                if(tga.fakeAlpha) {
                    texture.pixels[currentByte+3] = 255;
                } else if(texture.format == VK_FORMAT_R8G8B8A8_SRGB) {
                    texture.pixels[currentByte+3] = colorBuffer[3];
                }
                currentByte += texture.bytesPerPixel;
                currentPixel++;
            }
       } else {
            for(size_t i = 0; i <= chunkPixels; i++) {
                file.Fread(colorBuffer, 1, colorBufferSize);
                texture.pixels[currentByte] = colorBuffer[2];
                texture.pixels[currentByte+1] = colorBuffer[1];
                texture.pixels[currentByte+2] = colorBuffer[0];
                if(tga.fakeAlpha) {
                    texture.pixels[currentByte+3] = 255;
                } else if(texture.format == VK_FORMAT_R8G8B8A8_SRGB) {
                    texture.pixels[currentByte+3] = colorBuffer[3];
                }
                currentByte += texture.bytesPerPixel;
                currentPixel++;
            }
        }
    }
}

void loadColorMapInfoTga(Tga& tga, Texture& texture, unsigned char *head
    , const BinaryIfstream& file) {

    TgaColorMapInfo& colorMap = tga.colorMap;
    colorMap.type = head[1];
    colorMap.firstIndex = *((uint16_t*)(head+3));
    colorMap.length = *((uint16_t*)(head+5));
    colorMap.bitsPerElement = head[7];

    if(colorMap.type == 0 || tga.fakeAlpha) {
        return;
    }

    if(colorMap.bitsPerElement == 24) {
        texture.format = VK_FORMAT_R8G8B8_SRGB;
        texture.bytesPerPixel = 3;
    }
}

void loadInfoAboutTga(Tga& tga, Texture& texture
    , const BinaryIfstream& file, bool supportAlpha) {

    unsigned char headSize = 18;
    unsigned char head[headSize];
    file.Fread(head, sizeof(unsigned char), headSize);

    tga.dataType = head[2];
    tga.width = *((uint16_t*)(head+12));
    tga.height = *((uint16_t*)(head+14));
    tga.bitsPerPixel = head[16];
    tga.description = head[17]; 

    identifierTga(head[0], file);

    if(supportAlpha && tga.bitsPerPixel != 32) {
        tga.fakeAlpha = true;
        texture.bytesPerPixel = 4;
        texture.format = VK_FORMAT_R8G8B8A8_SRGB;
    } else {
        tga.fakeAlpha = false;
        texture.bytesPerPixel = tga.bitsPerPixel / 8;
        if(tga.bitsPerPixel == 24) {
            texture.format = VK_FORMAT_R8G8B8_SRGB;
        } else if(tga.bitsPerPixel == 32) {
            texture.format = VK_FORMAT_R8G8B8A8_SRGB;
        }
    }
    loadColorMapInfoTga(tga, texture, head, file);
    
    tga.imageBytesSize = texture.bytesPerPixel * tga.width * tga.height;
    texture.width = tga.width;
    texture.height = tga.height;
    texture.size = tga.imageBytesSize;
}

void identifierTga(unsigned char idLength, const BinaryIfstream& file) {
    if(idLength == 0) { return; }
    unsigned char id[idLength];
    file.Fread(id, sizeof(unsigned char), idLength);
}

void allocateImageMemoryTga(Tga& tga, Texture& texture
    , const BinaryIfstream& file) {
    if(tga.width <= 0 || tga.height <= 0) {
        throw std::invalid_argument("File [  ] is corrupted");
    }
    size_t colorSize = tga.colorMap.length * (tga.colorMap.bitsPerElement/8);
    texture.pixels = new unsigned char[tga.imageBytesSize + colorSize];

    if(tga.colorMap.type) {
        tga.colorMap.data = texture.pixels + tga.imageBytesSize;
        file.Fread(tga.colorMap.data, sizeof(unsigned char), colorSize);
    }
}
