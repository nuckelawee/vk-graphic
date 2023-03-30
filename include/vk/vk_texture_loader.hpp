#pragma once

#include "vk_image_builder.hpp"

namespace vk {

class TextureLoader {

public:

    Image Load(BufferBuilder& bufferBuilder, ImageBuilder& imageBuilder
        , const char *filepath);

    TextureLoader() {}
    ~TextureLoader() {}

};

} //vk
