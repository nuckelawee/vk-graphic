#pragma once 

#include "vk_device.hpp"

namespace vk {

class DescriptorPool {
    
    VkDescriptorPool pool_;

public:

    void Create(const Device& device);
    void Destroy(const Device& device);
    
    const VkDescriptorPool& Access() const;
    VkDescriptorPool& Access();

    DescriptorPool() {}
    ~DescriptorPool() {}
};

} // vk
