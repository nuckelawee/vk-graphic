#pragma once

#include "vk/vk_device.hpp"

namespace vk {

enum commandType { GRAPHICS, TRANSFER };

class CommandPool {

    VkCommandPool commandPool_;
    commandType type_;

public:

    void Create(const Device& device, commandType type);

    void Destroy(const Device& device);

    const VkCommandPool& Access() const { return commandPool_; }
    VkCommandPool& Access() { return commandPool_; }

    CommandPool() {}
    ~CommandPool() {}

};

} // vk
