#include "file_manager.hpp"
#include "tga.hpp"

std::string FileManager::ReadFile(const std::string& filepath
    , std::ios_base::openmode mode) {

    std::ifstream file(filepath, mode);
    if(file.is_open() == false) {
        ErrorManager::Validate(ERROR_TYPE_WARNING, "Failed to open file"
            , filepath);
        return "";
    }
    size_t fileSize = static_cast<size_t>(file.tellg());
    std::string code;
    code.resize(fileSize);
    file.seekg(0);
    file.read(code.data(), fileSize);

    file.close();
    return code;
}

FileResult FileManager::ReadImageTga(Texture& texture, const char *filepath
    , bool supportAlpha) {

    FILE *file = fopen(filepath, "rb");
    if(file == nullptr) { 
        return FILE_ERROR_OPEN_FILE;
    }
    
    Tga tga;
    loadInfoAboutTga(tga, texture, file, filepath, supportAlpha);
    FileResult result;

    switch(tga.dataType) {
    case TGA_FORMAT_NO_IMAGE:
        result = FILE_TGA_ERROR_NO_IMAGE;    
        break;
    case TGA_FORMAT_IMAGE_WITH_PALETTE:
        result = loadColorMapImageTga(tga, texture, file, filepath);
        break;
    case TGA_FORMAT_TRUE_COLOR_IMAGE:
        result = loadTrueColorImageTga(tga, texture, file, filepath);
        break;
    case TGA_FORMAT_RLE_IMAGE_WITH_PALETTE:
        result = loadRleColorMapImageTga(tga, texture, file, filepath);
        break;
    case TGA_FORMAT_RLE_TRUE_COLOR_IMAGE:
        result = loadRleTrueColorImageTga(tga, texture, file, filepath);
        break;
    case TGA_FORMAT_MONOCHROME_IMAGE:
    case TGA_FORMAT_RLE_MONOCHROME_IMAGE:
        result = FILE_TGA_ERROR_NOT_SUPPORT;
        break;
    default:
        result = FILE_TGA_ERROR_NOT_TGA;
    }
    fclose(file);
    return result;
}

FileResult loadColorMapImageTga(Tga& tga, Texture& texture, FILE *file
    , const char *filepath) {

    FileResult result = allocateImageMemoryTga(tga, texture, file);
    if(result != FILE_SUCCESS) { return result; } 

    uint32_t pixelCount = tga.width * tga.height;
    uint32_t currentByte = 0;
    unsigned char bytesPerElement = tga.colorMap.bitsPerElement / 8;
    unsigned char index;
    
    for(size_t currentPixel = 0; currentPixel < pixelCount; currentPixel++) {
        if(fread(&index, sizeof(unsigned char), 1, file) == 0) {
            return FILE_ERROR_TO_READ;
        }
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
    return FILE_SUCCESS;
}

FileResult loadRleColorMapImageTga(Tga& tga, Texture& texture, FILE *file
    , const char *filepath) {

    FileResult result = allocateImageMemoryTga(tga, texture, file);
    if(result != FILE_SUCCESS) { return result; } 

    uint32_t pixelCount = tga.width * tga.height;
    uint32_t currentPixel = 0;
    uint32_t currentByte = 0;
    unsigned char bytesPerElement = tga.colorMap.bitsPerElement / 8;
    unsigned char index;
    unsigned char chunk;

    while(currentPixel < pixelCount) {
        if(fread(&chunk, sizeof(unsigned char), 1, file) == 0) {
            return FILE_ERROR_TO_READ;
        }
        char isCompressed = chunk & 128;
        unsigned char chunkPixels = chunk & 127;
        if(isCompressed) {
            if(fread(&index, sizeof(unsigned char), 1, file) == 0) {
                return FILE_ERROR_TO_READ;
            }
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
                if(fread(&index, sizeof(unsigned char), 1, file) == 0) {
                    return FILE_ERROR_TO_READ;
                }

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
    return FILE_SUCCESS;
}

FileResult loadTrueColorImageTga(Tga& tga, Texture& texture, FILE *file
    , const char *filepath) {

    FileResult result = allocateImageMemoryTga(tga, texture, file);
    if(result != FILE_SUCCESS) { return result; } 

    if(tga.fakeAlpha) {
        unsigned char colorBuffer[3];
        uint32_t pixelCount = tga.width * tga.height;
        uint32_t currentByte = 0;
        for(size_t currentPixel = 0; currentPixel < pixelCount; currentPixel++) {
            if(fread(colorBuffer, 1, 3, file) == 0) {
                return FILE_ERROR_TO_READ;
            }
            texture.pixels[currentByte] = colorBuffer[2]; 
            texture.pixels[currentByte+1] = colorBuffer[1]; 
            texture.pixels[currentByte+2] = colorBuffer[0]; 
            texture.pixels[currentByte+3] = 255;
            currentByte += texture.bytesPerPixel;
        }
    } else {
        if(fread(texture.pixels, 1, tga.imageBytesSize, file) != tga.imageBytesSize) {
            return FILE_ERROR_TO_READ;    
        }
    } 
    return FILE_SUCCESS;
}

FileResult loadRleTrueColorImageTga(Tga& tga, Texture& texture, FILE *file
    , const char* filepath) {

    FileResult result = allocateImageMemoryTga(tga, texture, file);
    if(result != FILE_SUCCESS) { return result; } 

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
        if(fread(&chunkHeader, sizeof(unsigned char), 1, file) == 0) {
            return FILE_ERROR_TO_READ;    
        }
        char isCompressed = chunkHeader & 128;
        unsigned char chunkPixels = chunkHeader & 127;
        if(isCompressed) {
            if(fread(colorBuffer, 1, texture.bytesPerPixel, file) != texture.bytesPerPixel) {
                return FILE_ERROR_TO_READ;   
            }
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
                if(fread(colorBuffer, 1, texture.bytesPerPixel
                    , file) != texture.bytesPerPixel)
                { return FILE_ERROR_TO_READ; }
                texture.pixels[currentByte] = colorBuffer[0];
                texture.pixels[currentByte+1] = colorBuffer[1];
                texture.pixels[currentByte+2] = colorBuffer[2];
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
    return FILE_SUCCESS;
}

void loadColorMapInfoTga(Tga& tga, Texture& texture, unsigned char *head
    , FILE *file) {

    TgaColorMapInfo& colorMap = tga.colorMap;
    colorMap.colorMapType = head[1];
    colorMap.firstIndex = *((uint16_t*)(head+3));
    colorMap.length = *((uint16_t*)(head+5));
    colorMap.bitsPerElement = head[7];

    if(colorMap.colorMapType == 0 || tga.fakeAlpha) {
        return;
    }

    if(colorMap.bitsPerElement == 24) {
        texture.format = VK_FORMAT_R8G8B8_SRGB;
        texture.bytesPerPixel = 3;
    }
}

FileResult loadInfoAboutTga(Tga& tga, Texture& texture
    , FILE *file, const char *filepath, bool supportAlpha) {

    unsigned char headSize = 18;
    unsigned char head[headSize];
    if(fread(head, sizeof(unsigned char), headSize, file) == 0) {
        return FILE_ERROR_TO_READ; 
    }
    tga.dataType = head[2];
    tga.width = *((uint16_t*)(head+12));
    tga.height = *((uint16_t*)(head+14));
    tga.bitsPerPixel = head[16];

    FileResult result = identifierTga(head[0], file);
    if(result != FILE_SUCCESS) { return result; }

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
    
    texture.width = tga.width;
    texture.height = tga.height;
    tga.imageBytesSize = texture.bytesPerPixel * tga.width * tga.height;
    return FILE_SUCCESS;
}

FileResult identifierTga(unsigned char idLength, FILE *file) {
    if(idLength == 0) { return FILE_SUCCESS; }
    unsigned char id[idLength];
    if(fread(id, sizeof(unsigned char), idLength, file) == 0) {
        return FILE_ERROR_TO_READ;
    }
    return FILE_SUCCESS;
}

FileResult allocateImageMemoryTga(Tga& tga, Texture& texture, FILE *file) {
   if(tga.width <= 0 || tga.height <= 0) {
        return FILE_TGA_ERROR_FILE_IS_CORRUPTED;
    }

    size_t colorSize = tga.colorMap.length * (tga.colorMap.bitsPerElement/8);
    texture.pixels = new unsigned char[tga.imageBytesSize + colorSize];
    if(texture.pixels == nullptr) {
        return FILE_ERROR_MEMORY_ALLOCATION;
    }

    if(tga.colorMap.colorMapType) {
        tga.colorMap.data = texture.pixels + tga.imageBytesSize;
        if(fread(tga.colorMap.data, sizeof(unsigned char), colorSize, file) == 0) {
            return FILE_ERROR_TO_READ;
        }
    }
    return FILE_SUCCESS;
}
