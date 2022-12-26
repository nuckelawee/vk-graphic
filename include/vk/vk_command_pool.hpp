#pragma once

#include "vk/vk_device.hpp"

namespace vk {

enum commandPoolType { GRAPHICS, TRANSFER };

class CommandPool {

    VkCommandPool commandPool_;

public:

    void Create(const Device& device, commandPoolType type = GRAPHICS);

    void Destroy(const Device& device);

    VkCommandPool Access() const { return commandPool_; }

    CommandPool() {}
    ~CommandPool() {}

};

} // vk
