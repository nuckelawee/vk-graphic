#include "vk_extensions.hpp"

#ifdef DEBUG

void VulkanLayersAndExtensions::PrintAvailableLayersAndExtensions(
    const VkPhysicalDevice& gpu) {

    GetInstanceLayerProperties();
    GetDeviceLayerProperties(gpu);
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
    const VkPhysicalDevice& gpu) {

    std::vector<LayerProperties> gpuLayerProperties;
    uint32_t layerCount;
    VkResult result;

    result = vkEnumerateDeviceLayerProperties(gpu, &layerCount, nullptr);
    if(layerCount == 0) { return result; }
    
    VkLayerProperties pProperties[layerCount];
    result = vkEnumerateDeviceLayerProperties(gpu, &layerCount, pProperties);
    gpuLayerProperties.resize(layerCount);

    for(size_t i = 0; i < layerCount; i++) {
        gpuLayerProperties[i].layer = pProperties[i];
    }
    GetDeviceExtensionProperties(gpuLayerProperties, gpu);

    PrintLayerDescriptions(gpuLayerProperties);
    return result;
}

void VulkanLayersAndExtensions::GetDeviceExtensionProperties(
      std::vector<LayerProperties>& layerProperties
    , const VkPhysicalDevice& gpu) {

    for(size_t i = 0; i < layerProperties.size(); i++) {
        const char* layerName = layerProperties[i].layer.layerName;
        uint32_t extensionsCount;

        vkEnumerateDeviceExtensionProperties(gpu, layerName, &extensionsCount
            , nullptr);
        if(extensionsCount == 0) { continue; }
        layerProperties[i].extensions.resize(extensionsCount);
        vkEnumerateDeviceExtensionProperties(gpu, layerName, &extensionsCount
            , layerProperties[i].extensions.data());
    }
}
#endif

int32_t VulkanLayersAndExtensions::LayerExist(const char* pLayer
    , const VkLayerProperties *pAvailableLayers
    , uint32_t layerCount) const {
    
    for(uint32_t i = 0; i < layerCount; i++) {
        if(strcmp(pLayer, pAvailableLayers[i].layerName) == 0)
        { return i; }
    }
    return -1;
}

int32_t VulkanLayersAndExtensions::ExtensionExist(const char* pExtension
    , const VkExtensionProperties *pAvailableExtensions
    , uint32_t extensionCount) const {

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
    if(layerCount == 0 || layerCount < layers.size()) {
#ifdef DEBUG
        std::cerr << "\nWARNING [ Instance layer request ]\n---> "\
            "Layer not present\n\n";
#endif
        return VK_ERROR_LAYER_NOT_PRESENT;
    }

    VkLayerProperties pAvailableLayers[layerCount];
    vkEnumerateInstanceLayerProperties(&layerCount, pAvailableLayers);

    for(size_t i = 0; i < layers.size(); i++) {
        int32_t layerIndex = LayerExist(layers[i], pAvailableLayers, layerCount);
        if(layerIndex == -1) {
#ifdef DEBUG
            std::cerr << "\nWARNING [ Instance layer request ]\n---> "\
                "Layer not present\n\n";
#endif
            return VK_ERROR_LAYER_NOT_PRESENT;
        }
        enableInstanceLayers.push_back(pAvailableLayers[layerIndex]);
    }
    return VK_SUCCESS;
}

VkResult VulkanLayersAndExtensions::RequestInstanceExtensions(
      const std::vector<const char*>& extensions) {

    uint32_t extensionCount;
    vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
    if(extensionCount == 0 || extensionCount < extensions.size()) {
#ifdef DEBUG
        std::cerr << "\nWARNING [ Instance extension request ]\n---> "\
            "Extension not present\n\n";
#endif
        return VK_ERROR_EXTENSION_NOT_PRESENT;
    }

    VkExtensionProperties pAvailableExtensions[extensionCount];
    vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount
        , pAvailableExtensions);

    for(size_t i = 0; i < extensions.size(); i++) {
        int32_t extensionIndex = ExtensionExist(extensions[i]
            , pAvailableExtensions, extensionCount);
        if(extensionIndex == -1) {
#ifdef DEBUG
            std::cerr << "\nWARNING [ Instance extension request ]\n---> "\
                "Extension not present\n\n";
#endif
            return VK_ERROR_EXTENSION_NOT_PRESENT;
        }
        enableInstanceExtensions.push_back(pAvailableExtensions[extensionIndex]);
    }
    return VK_SUCCESS;
}

/*
VkResult VulkanLayersAndExtensions::RequestDeviceLayers(
      const std::vector<const char*>& layers, const VkPhysicalDevice& gpu) {

    uint32_t layerCount;
    vkEnumerateDeviceLayerProperties(gpu, &layerCount, nullptr);
    if(layerCount == 0 || layerCount < layers.size()) {
#ifdef DEBUG
        std::cerr << "\nWARNING [ Device layer request ]\n---> "\
            "Layer not present\n\n";
#endif
        return VK_ERROR_LAYER_NOT_PRESENT;
    }

    VkLayerProperties pAvailableLayers[layerCount];
    vkEnumerateDeviceLayerProperties(gpu, &layerCount, pAvailableLayers);

    for(size_t i = 0; i < layers.size(); i++) {
        int32_t layerIndex = LayerExist(layers[i], pAvailableLayers, layerCount);
        if(layerIndex == -1) {
#ifdef DEBUG
            std::cerr << "\nWARNING [ Device layer request ]\n---> "\
                "Layer not present\n\n";
#endif
            return VK_ERROR_LAYER_NOT_PRESENT;
        }
        enableDeviceLayers.push_back(pAvailableLayers[layerIndex]);
    }
    return VK_SUCCESS;
}
*/

VkResult VulkanLayersAndExtensions::RequestDeviceExtensions(
      const std::vector<const char*>& extensions, const VkPhysicalDevice& gpu) {

    uint32_t extensionCount;
    vkEnumerateDeviceExtensionProperties(gpu, nullptr, &extensionCount, nullptr);
    if(extensionCount == 0 || extensionCount < extensions.size()) {
#ifdef DEBUG
        std::cerr << "\nWARNING [ Device extension request ]\n---> "\
            "Extension not present\n\n";
#endif
        return VK_ERROR_EXTENSION_NOT_PRESENT;
    }

    VkExtensionProperties pAvailableExtensions[extensionCount];
    vkEnumerateDeviceExtensionProperties(gpu, nullptr, &extensionCount
        , pAvailableExtensions);

    for(size_t i = 0; i < extensions.size(); i++) {
        int32_t extensionIndex = ExtensionExist(extensions[i]
            , pAvailableExtensions, extensionCount);
        if(extensionIndex == -1) {
#ifdef DEBUG
            std::cerr << "\nWARNING [ Device extension request ]\n---> "\
                "Extension not present\n\n";
#endif
            return VK_ERROR_EXTENSION_NOT_PRESENT;
        }
        enableDeviceExtensions.push_back(pAvailableExtensions[extensionIndex]);
    }
    return VK_SUCCESS;
}

const std::vector<VkLayerProperties>&
    VulkanLayersAndExtensions::GetInstanceLayers() const
{ return enableInstanceLayers; }

std::vector<const char*> VulkanLayersAndExtensions::GetInstanceLayerNames() const {
    std::vector<const char*> layerNames(enableInstanceLayers.size());
    for(size_t i = 0; i < layerNames.size(); i++) {
        layerNames[i] = enableInstanceLayers[i].layerName;
    }
    return layerNames;
}
    
const std::vector<VkExtensionProperties>&
    VulkanLayersAndExtensions::GetInstanceExtensions() const
{ return enableInstanceExtensions; }

std::vector<const char*> VulkanLayersAndExtensions::GetInstanceExtensionNames() const {
    std::vector<const char*> extensionNames(enableInstanceExtensions.size());
    for(size_t i = 0; i < extensionNames.size(); i++) {
        extensionNames[i] = enableInstanceExtensions[i].extensionName;
    }
    return extensionNames;
}
   
/*
const std::vector<VkLayerProperties>& 
    VulkanLayersAndExtensions::GetDeviceLayers() const
{ return enableDeviceLayers; }

std::vector<const char*> VulkanLayersAndExtensions::GetDeviceLayerNames() const {
    std::vector<const char*> layerNames(enableDeviceLayers.size());
    for(size_t i = 0; i < layerNames.size(); i++) {
        layerNames[i] = enableDeviceLayers[i].layerName;
    }
    return layerNames;
}
*/

const std::vector<VkExtensionProperties>&
    VulkanLayersAndExtensions::GetDeviceExtensions() const
{ return enableDeviceExtensions; }

std::vector<const char*> VulkanLayersAndExtensions::GetDeviceExtensionNames() const {
    std::vector<const char*> extensionNames(enableDeviceExtensions.size());
    for(size_t i = 0; i < extensionNames.size(); i++) {
        extensionNames[i] = enableDeviceExtensions[i].extensionName;
    }
    return extensionNames;
}

