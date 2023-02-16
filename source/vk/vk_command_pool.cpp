#include "vk/vk_command_pool.hpp"

namespace vk {

void CommandPool::Create(const Device& device, commandType type) {
    type_ = type;
    uint32_t queueFamilyIndex; 
    VkCommandPoolCreateFlags flags;
    switch(type_) {
    case COMMAND_TYPE_GRAPHICS:
        queueFamilyIndex = device.AccessQueues().graphic.index.value();
        flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
        break;
    case COMMAND_TYPE_TRANSFER:
        queueFamilyIndex = device.AccessQueues().transfer.index.value();
        flags = VK_COMMAND_POOL_CREATE_TRANSIENT_BIT
            | VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
        break;
    default:
        ErrorManager::Validate(UNSOLVABLE, "Trying to create unsupportable "\
            "command pool", "Command pool creation");
    }
    
    VkCommandPoolCreateInfo commandPoolInfo {};
    commandPoolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    commandPoolInfo.flags = flags;
    commandPoolInfo.queueFamilyIndex = queueFamilyIndex;
    VkResult result = vkCreateCommandPool(device.Access(), &commandPoolInfo
        , nullptr, &commandPool_);
    ErrorManager::Validate(result, "Command pool creation");
}

void CommandPool::Destroy(const Device& device) {
    vkDestroyCommandPool(device.Access(), commandPool_, nullptr);
}

} //vk
