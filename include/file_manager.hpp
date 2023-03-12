#pragma once

#include <fstream>
#include <cstdlib>
#include <cstring>

#include "error_manager.hpp"

struct Texture {
    unsigned char *pixels;   
    int width;
    int height;
    unsigned char bytesPerPixel;
    VkFormat format; 
};

class FileManager {

public:

    static std::string ReadFile(const std::string& filepath, std::ios_base::openmode);

    static FileResult ReadImageTga(Texture& texture, const char *filepath
        , bool supportAlpha = true);

};
