#pragma once

#include <vulkan/vulkan.h>
#include <vector>
#include <cstring>

#ifdef DEBUG
#include <iostream>

struct LayerProperties {
    VkLayerProperties   layer;
    std::vector<VkExtensionProperties> extensions;
};
#endif

class VulkanLayersAndExtensions {

    std::vector<VkLayerProperties> enableInstanceLayers;
    std::vector<VkExtensionProperties> enableInstanceExtensions;
    std::vector<VkLayerProperties> enableDeviceLayers;
    std::vector<VkExtensionProperties> enableDeviceExtensions;

public:

    VkResult RequestInstanceLayers(const std::vector<const char*>& layers);
    VkResult RequestInstanceExtensions(const std::vector<const char*>& extensions);
    VkResult RequestDeviceLayers(const std::vector<const char*>& layers
        , VkPhysicalDevice device);
    VkResult RequestDeviceExtensions(const std::vector<const char*>& extensions
        , VkPhysicalDevice device);

    int32_t LayerExist(const char* layer
        , const VkLayerProperties *pAvailableLayers, uint32_t layerCount);
    int32_t ExtensionExist(const char* extension
        , const VkExtensionProperties *pAvailableExtensions
        , uint32_t extensionCount);

    const std::vector<VkLayerProperties>& GetInstanceLayers()
    { return enableInstanceLayers; }
    const std::vector<VkExtensionProperties> GetInstsanceExtensions()
    { return enableInstanceExtensions; }
    const std::vector<VkLayerProperties> GetDeviceLayers()
    { return enableDeviceLayers; }
    const std::vector<VkExtensionProperties> GetDeviceExtensions()
    { return enableDeviceExtensions; }

#ifdef DEBUG
    static void PrintAvailableLayersAndExtensions(VkPhysicalDevice device);

private:
    static VkResult GetInstanceLayerProperties();
    static void GetInstanceExtensionProperties(
        std::vector<LayerProperties>& layerProperties);

    static VkResult GetDeviceLayerProperties(VkPhysicalDevice device);
    static void GetDeviceExtensionProperties(
        std::vector<LayerProperties>& layerProperties, VkPhysicalDevice device);

    static VkResult GetDeviceExtensionProperties(VkPhysicalDevice *pGpu);
    static void PrintLayerDescriptions(const std::vector<LayerProperties>&);
#endif
};
