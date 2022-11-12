#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <vector>
#include <cstring>
#include <iostream>
#include <cassert>

namespace vk {

#ifdef DEBUG

struct LayerProperties {
    VkLayerProperties   layer;
    std::vector<VkExtensionProperties> extensions;
};
#endif

class VulkanLayersAndExtensions {

    std::vector<VkLayerProperties> enableInstanceLayers;
    std::vector<VkExtensionProperties> enableInstanceExtensions;
    // deprecated
    //std::vector<VkLayerProperties> enableDeviceLayers; 
    std::vector<VkExtensionProperties> enableDeviceExtensions;

public:

    VkResult RequestInstanceLayers(const std::vector<const char*>& layers);
    VkResult RequestInstanceExtensions(const std::vector<const char*>& extensions);
/*
    VkResult RequestDeviceLayers(const std::vector<const char*>& layers
        , const VkPhysicalDevice& gpu);
*/
    VkResult RequestDeviceExtensions(const std::vector<const char*>& extensions
        , const VkPhysicalDevice& gpu);

    int32_t LayerExist(const char* layer
        , const VkLayerProperties *pAvailableLayers, uint32_t layerCount) const;
    int32_t ExtensionExist(const char* extension
        , const VkExtensionProperties *pAvailableExtensions
        , uint32_t extensionCount) const;

    const std::vector<VkLayerProperties>& GetInstanceLayers() const;
    std::vector<const char*> GetInstanceLayerNames() const;

    const std::vector<VkExtensionProperties>& GetInstanceExtensions() const;
    std::vector<const char*> GetInstanceExtensionNames() const;

    const std::vector<VkLayerProperties>& GetDeviceLayers() const;
    std::vector<const char*> GetDeviceLayerNames() const;

    const std::vector<VkExtensionProperties>& GetDeviceExtensions() const;
    std::vector<const char*> GetDeviceExtensionNames() const;

#ifdef DEBUG
    static void PrintAvailableLayersAndExtensions(const VkPhysicalDevice& gpu);

private:
    static VkResult GetInstanceLayerProperties();
    static void GetInstanceExtensionProperties(
        std::vector<LayerProperties>& layerProperties);

    static VkResult GetDeviceLayerProperties(const VkPhysicalDevice& gpu);
    static void GetDeviceExtensionProperties(
          std::vector<LayerProperties>& layerProperties
        , const VkPhysicalDevice& gpu);

    static VkResult GetDeviceExtensionProperties(const VkPhysicalDevice& gpu);
    static void PrintLayerDescriptions(const std::vector<LayerProperties>&);
#endif
};

}
