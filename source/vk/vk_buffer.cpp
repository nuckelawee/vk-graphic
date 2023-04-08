#include "vk/vk_buffer.hpp"

namespace vk {

void Buffer::Destroy(VkDevice device) {
    vkDestroyBuffer(device, buffer, nullptr);
    vkFreeMemory(device, memory, nullptr);
}

}
