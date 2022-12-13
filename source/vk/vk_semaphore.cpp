#include "vk/vk_semaphore.hpp"

namespace vk {

void Semaphore::Create(const Device& device) {
    VkSemaphoreCreateInfo semaphoreInfo {};
    semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
    VkResult result = vkCreateSemaphore(device.Access()
        , &semaphoreInfo, nullptr, &semaphore_);
    ErrorManager::Validate(result, "Semaphore creation");
}

void Semaphore::Destroy(const Device& device) {
    vkDestroySemaphore(device.Access(), semaphore_, nullptr); 
}

} //vk
