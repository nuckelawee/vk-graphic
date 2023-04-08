#pragma once

#include <fstream>
#include <cstdlib>
#include <cstring>

#include "error_manager.hpp"
#include "texture.hpp"

struct Texture;

class FileManager {

public:

    static std::string ReadFile(const std::string& filepath, std::ios_base::openmode);

    static Texture ReadImageTga(const std::string& filepath
        , bool supportAlpha = true);

};
