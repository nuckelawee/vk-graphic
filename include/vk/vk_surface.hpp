#pragma once

#include <iostream>
#include <cassert>
#include <vector>

#include "vk_instance.hpp"

namespace vk {

struct SurfaceDetails {
    VkSurfaceCapabilitiesKHR capabilities {};
    std::vector<VkSurfaceFormatKHR> formats {};
    std::vector<VkPresentModeKHR> presentModes {};
};

class Surface {
protected:

    VkSurfaceKHR surface_;

public:
    virtual VkResult Create(const Instance& instance)
    { return VK_ERROR_NATIVE_WINDOW_IN_USE_KHR; }

    virtual SurfaceDetails Capabilities(const VkPhysicalDevice& gpu) const
    { return SurfaceDetails{}; }

    VkSurfaceKHR Access() const { return surface_; }

    void Destroy(const Instance& instance);
 
    Surface() {}
    ~Surface() {}
};

} //vk
