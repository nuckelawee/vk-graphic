#include "vk/vk_image.hpp"

namespace vk {

void Image::Destroy(VkDevice device) {
    vkDestroyImageView(device, model.image.view, nullptr);
    vkDestroyImage(device, model.image.image, nullptr);
    vkDestroySampler(device, model.image.sampler, nullptr);
    vkFreeMemory(device, model.image.memory, nullptr); 
}

}
