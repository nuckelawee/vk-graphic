#pragma once

#include "vk/vk_device.hpp"

#include "error_manager.hpp"

namespace vk {

class Fence {

    VkFence fence_;

public:

    void Create(const Device& device);
    void Destroy(const Device& device);

    VkFence& Access() { return fence_; }

    Fence() {}
    ~Fence() {}

};

}// vk
