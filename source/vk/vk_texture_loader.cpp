#include "vk/vk_texture_loader.hpp"
#include "file_manager.hpp"

namespace vk {

Image TextureLoader::Load(BufferBuilder& bufferBuilder
    , ImageBuilder& imageBuilder, const char *filepath) {

    Texture texture;
    FileResult result = FileManager::ReadImageTga(texture, filepath);
    ErrorManager::Validate(result, "Texture loading");

    return imageBuilder.BuildImage(bufferBuilder, texture);
}

} //vk
