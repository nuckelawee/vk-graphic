#include "vk/vk_command_pool.hpp"

namespace vk {

void CommandPool::Create(const Device& device) {
    VkCommandPoolCreateInfo commandPoolInfo {};
    commandPoolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    commandPoolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    commandPoolInfo.queueFamilyIndex = device.AccessQueues().graphic.index.value();
    VkResult result = vkCreateCommandPool(device.Access(), &commandPoolInfo
        , nullptr, &commandPool_);
    ErrorManager::Validate(result, "Command pool creation");
}

void CommandPool::Destroy(const Device& device) {
    vkDestroyCommandPool(device.Access(), commandPool_, nullptr);
}

} //vk
