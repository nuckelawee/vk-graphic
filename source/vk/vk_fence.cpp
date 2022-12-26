#include "vk/vk_fence.hpp"

namespace vk {

void Fence::Create(const Device& device) {
    VkFenceCreateInfo fenceInfo {};
    fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;
    VkResult result = vkCreateFence(device.Access(), &fenceInfo
        , nullptr, &fence_);
    ErrorManager::Validate(result, "Fence creation");
}

void Fence::Destroy(const Device& device) {
    vkDestroyFence(device.Access(), fence_, nullptr); 
}

} //vk
