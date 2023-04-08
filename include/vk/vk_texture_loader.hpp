#pragma once

#include "vk_image_builder.hpp"

namespace vk {

class TextureLoader {
    constexpr static const char *missed_texture = "resources/missed_texture.tga";
public:

    Image Load(BufferBuilder& bufferBuilder, ImageBuilder& imageBuilder
        , const char *filepath);

};

} //vk
