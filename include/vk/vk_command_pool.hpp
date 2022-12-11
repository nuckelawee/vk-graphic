#pragma once

#include "vk/vk_device.hpp"

namespace vk {

class CommandPool {

    VkCommandPool commandPool_;

public:

    void Create(const Device& device);
    void Destroy(const Device& device);

    VkCommandPool Access() const { return commandPool_; }

};

} // vk
