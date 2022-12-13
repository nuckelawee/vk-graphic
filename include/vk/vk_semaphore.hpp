#pragma once

#include "vk/vk_device.hpp"

#include "error_manager.hpp"

namespace vk {

class Semaphore {

    VkSemaphore semaphore_;

public:

    void Create(const Device& device);
    void Destroy(const Device& device);

    VkSemaphore& Access() { return semaphore_; }

    Semaphore() {}
    ~Semaphore() {}

};

}// vk
