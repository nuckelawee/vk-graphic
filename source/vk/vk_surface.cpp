#include "vk/vk_surface.hpp"
#include "vk/vk_exception.hpp"

namespace vk {

Surface::Surface(VkInstance instance) : Window(), instance_(instance) {
    VkResult result = glfwCreateWindowSurface(instance_, window_
        , nullptr, &surface_);
    if(result != VK_SUCCESS) {
        throw Exception("Failed to create GLFW window surface", result);
    } 
}

SurfaceDetails Surface::Capabilities(VkPhysicalDevice gpu) const noexcept {
    vk::SurfaceDetails details;

    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(gpu, surface_
        , &details.capabilities);

    uint32_t formatCount = 0;
    vkGetPhysicalDeviceSurfaceFormatsKHR(gpu, surface_, &formatCount, nullptr);
    if(formatCount > 0) { 
        details.formats.resize(formatCount);
        vkGetPhysicalDeviceSurfaceFormatsKHR(gpu, surface_, &formatCount
            , details.formats.data());
    }

    uint32_t presentCount;
    vkGetPhysicalDeviceSurfacePresentModesKHR(gpu, surface_, &presentCount
        , nullptr);
    if(presentCount > 0) {
        details.presentModes.resize(presentCount);
        vkGetPhysicalDeviceSurfacePresentModesKHR(gpu, surface_, &presentCount
            , details.presentModes.data());
    } 
    return details;
}

VkSurfaceKHR Surface::Access() const noexcept { return surface_; }

Surface::~Surface() {
    vkDestroySurfaceKHR(instance_, surface_, nullptr);
}

} // vk
