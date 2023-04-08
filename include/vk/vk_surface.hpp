#pragma once

#include <vector>
#include "vk_instance.hpp"
#include "window.hpp"

namespace vk {

struct SurfaceDetails {
    VkSurfaceCapabilitiesKHR capabilities {};
    std::vector<VkSurfaceFormatKHR> formats {};
    std::vector<VkPresentModeKHR> presentModes {};
};

class Surface : public Window {

    VkSurfaceKHR surface_;
    VkInstance instance_;

public:

    Surface(VkInstance instance);

    SurfaceDetails Capabilities(VkPhysicalDevice gpu) const noexcept;

    VkSurfaceKHR Access() const noexcept;

    virtual ~Surface(); 
    
    Surface(Surface&&) = default;
    Surface& operator=(Surface&&) = default;
};

} //vk
