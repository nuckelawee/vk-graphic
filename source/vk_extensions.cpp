#include "vk_extensions.hpp"

#ifdef DEBUG

void VulkanLayersAndExtensions::PrintAvailableLayersAndExtensions(
      VkPhysicalDevice device) {

    GetInstanceLayerProperties();
    GetDeviceLayerProperties(device);
}

void VulkanLayersAndExtensions::PrintLayerDescriptions(
    const std::vector<LayerProperties>& layerProperties) {

    for(LayerProperties properties : layerProperties) {
        std::cout << '\n' << properties.layer.description << "\n\t---[Layer name]--> "
            << properties.layer.layerName << '\n';
        for(size_t i = 0; i < properties.extensions.size(); i++) {
            std::cout << "\t\t---[Layer extensions]--> "
                << properties.extensions[i].extensionName << '\n';
        }
    }
}

VkResult VulkanLayersAndExtensions::GetInstanceLayerProperties() {

    std::vector<LayerProperties> instanceLayerProperties;
    uint32_t layerCount;
    VkResult result;
    result = vkEnumerateInstanceLayerProperties(&layerCount, nullptr);
    if(layerCount == 0) { return result; }

    VkLayerProperties pProperties[layerCount];
    vkEnumerateInstanceLayerProperties(&layerCount, pProperties);

    instanceLayerProperties.resize(layerCount);
    for(size_t i = 0; i < layerCount; i++) {
        instanceLayerProperties[i].layer = pProperties[i];
    }
    GetInstanceExtensionProperties(instanceLayerProperties);
    PrintLayerDescriptions(instanceLayerProperties);
    return result;
}

void VulkanLayersAndExtensions::GetInstanceExtensionProperties(
    std::vector<LayerProperties>& layerProperties) {

    for(size_t i = 0; i < layerProperties.size(); i++) {
        const char* layerName = layerProperties[i].layer.layerName;
        uint32_t extensionsCount;

        vkEnumerateInstanceExtensionProperties(layerName, &extensionsCount,
            nullptr);
        if(extensionsCount == 0) { continue; } 
        layerProperties[i].extensions.resize(extensionsCount);
        vkEnumerateInstanceExtensionProperties(layerName, &extensionsCount
            , layerProperties[i].extensions.data());
    }
}

VkResult VulkanLayersAndExtensions::GetDeviceLayerProperties(
      VkPhysicalDevice device) {

    std::vector<LayerProperties> deviceLayerProperties;
    uint32_t layerCount;
    VkResult result;

    result = vkEnumerateDeviceLayerProperties(device, &layerCount, nullptr);
    if(layerCount == 0) { return result; }
    
    VkLayerProperties pProperties[layerCount];
    result = vkEnumerateDeviceLayerProperties(device, &layerCount, pProperties);
    deviceLayerProperties.resize(layerCount);

    for(size_t i = 0; i < layerCount; i++) {
        deviceLayerProperties[i].layer = pProperties[i];
    }
    GetDeviceExtensionProperties(deviceLayerProperties, device);

    PrintLayerDescriptions(deviceLayerProperties);
    return result;
}

void VulkanLayersAndExtensions::GetDeviceExtensionProperties(
      std::vector<LayerProperties>& layerProperties, VkPhysicalDevice device) {

    for(size_t i = 0; i < layerProperties.size(); i++) {
        const char* layerName = layerProperties[i].layer.layerName;
        uint32_t extensionsCount;

        vkEnumerateDeviceExtensionProperties(device, layerName, &extensionsCount
            , nullptr);
        if(extensionsCount == 0) { continue; }
        layerProperties[i].extensions.resize(extensionsCount);
        vkEnumerateDeviceExtensionProperties(device, layerName, &extensionsCount
            , layerProperties[i].extensions.data());
    }
}
#endif

int32_t VulkanLayersAndExtensions::LayerExist(const char* pLayer
    , const VkLayerProperties *pAvailableLayers, uint32_t layerCount) {
    
    for(uint32_t i = 0; i < layerCount; i++) {
        if(strcmp(pLayer, pAvailableLayers[i].layerName) == 0)
        { return i; }
    }
    return -1;
}

int32_t VulkanLayersAndExtensions::ExtensionExist(const char* pExtension
    , const VkExtensionProperties *pAvailableExtensions, uint32_t extensionCount) {

    for(uint32_t i = 0; i < extensionCount; i++) {
        if(strcmp(pExtension, pAvailableExtensions[i].extensionName) == 0)
        { return i; }
    }
    return -1;
}

VkResult VulkanLayersAndExtensions::RequestInstanceLayers(
      const std::vector<const char*>& layers) {

    uint32_t layerCount;
    vkEnumerateInstanceLayerProperties(&layerCount, nullptr);
    if(layerCount == 0 || layerCount < layers.size())
    { return VK_ERROR_LAYER_NOT_PRESENT; }

    VkLayerProperties pAvailableLayers[layerCount];
    vkEnumerateInstanceLayerProperties(&layerCount, pAvailableLayers);

    for(size_t i = 0; i < layers.size(); i++) {
        if(LayerExist(layers[i], pAvailableLayers, layerCount) == -1)
        { return VK_ERROR_LAYER_NOT_PRESENT; }
        enableInstanceLayers.push_back(pAvailableLayers[i]);
    }
    return VK_SUCCESS;
}

VkResult VulkanLayersAndExtensions::RequestInstanceExtensions(
      const std::vector<const char*>& extensions) {

    uint32_t extensionCount;
    vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
    if(extensionCount == 0 || extensionCount < extensions.size())
    { return VK_ERROR_EXTENSION_NOT_PRESENT; }

    VkExtensionProperties pAvailableExtensions[extensionCount];
    vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount
        , pAvailableExtensions);

    for(size_t i = 0; i < extensions.size(); i++) {
        if(ExtensionExist(extensions[i], pAvailableExtensions, extensionCount) == -1)
        { return VK_ERROR_EXTENSION_NOT_PRESENT; }
        enableInstanceExtensions.push_back(pAvailableExtensions[i]);
    }
    return VK_SUCCESS;
}

VkResult VulkanLayersAndExtensions::RequestDeviceLayers(
      const std::vector<const char*>& layers, VkPhysicalDevice device) {

    uint32_t layerCount;
    vkEnumerateDeviceLayerProperties(device, &layerCount, nullptr);
    if(layerCount == 0 || layerCount < layers.size())
    { return VK_ERROR_LAYER_NOT_PRESENT; }

    VkLayerProperties pAvailableLayers[layerCount];
    vkEnumerateDeviceLayerProperties(device, &layerCount, pAvailableLayers);

    for(size_t i = 0; i < layers.size(); i++) {
        if(LayerExist(layers[i], pAvailableLayers, layerCount) == -1)
        { return VK_ERROR_LAYER_NOT_PRESENT; }
        enableDeviceLayers.push_back(pAvailableLayers[i]);
    }
    return VK_SUCCESS;
}

VkResult VulkanLayersAndExtensions::RequestDeviceExtensions(
      const std::vector<const char*>& extensions, VkPhysicalDevice device) {

    uint32_t extensionCount;
    vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);
    if(extensionCount == 0 || extensionCount < extensions.size())
    { return VK_ERROR_EXTENSION_NOT_PRESENT; }

    VkExtensionProperties pAvailableExtensions[extensionCount];
    vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount
        , pAvailableExtensions);

    for(size_t i = 0; i < extensions.size(); i++) {
        if(ExtensionExist(extensions[i], pAvailableExtensions, extensionCount) == -1)
        { return VK_ERROR_EXTENSION_NOT_PRESENT; }
        enableDeviceExtensions.push_back(pAvailableExtensions[i]);
    }
    return VK_SUCCESS;
}
