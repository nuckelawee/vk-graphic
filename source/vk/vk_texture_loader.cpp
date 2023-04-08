#include "vk/vk_texture_loader.hpp"
#include "file_manager.hpp"
#include "vk/vk_image.hpp"

namespace vk {

Image TextureLoader::Load(BufferBuilder& bufferBuilder
    , ImageBuilder& imageBuilder, const char *filepath) {

    Texture texture;
    try {
        texture = FileManager::ReadImageTga(filepath);
    } catch(std::invalid_argument& info) {
        std::cerr << info.what() << '\n';
        texture = FileManager::ReadImageTga(TextureLoader::missed_texture);
    } catch(std::exception& info) {
        std::cerr << info.what() << '\n';
        exit(EXIT_FAILURE);
    }

    return imageBuilder.BuildImage(bufferBuilder, texture);
}

} //vk
