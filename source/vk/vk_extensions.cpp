#include "vk/vk_extensions.hpp"

namespace vk {

#ifdef DEBUG

void LayersAndExtensions::PrintAvailableLayersAndExtensions(
    const VkPhysicalDevice& gpu) {

    GetInstanceLayerProperties();
    GetDeviceLayerProperties(gpu);
}

void LayersAndExtensions::PrintLayerDescriptions(
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

VkResult LayersAndExtensions::GetInstanceLayerProperties() {

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

void LayersAndExtensions::GetInstanceExtensionProperties(
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

VkResult LayersAndExtensions::GetDeviceLayerProperties(
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

void LayersAndExtensions::GetDeviceExtensionProperties(
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

int32_t LayersAndExtensions::LayerExist(const char* pLayer
    , const VkLayerProperties *pAvailableLayers
    , uint32_t layerCount) const {
    
    for(uint32_t i = 0; i < layerCount; i++) {
        if(strcmp(pLayer, pAvailableLayers[i].layerName) == 0)
        { return i; }
    }
    return -1;
}

int32_t LayersAndExtensions::ExtensionExist(const char* pExtension
    , const VkExtensionProperties *pAvailableExtensions
    , uint32_t extensionCount) const {

    for(uint32_t i = 0; i < extensionCount; i++) {
        if(strcmp(pExtension, pAvailableExtensions[i].extensionName) == 0)
        { return i; }
    }
    return -1;
}

VkResult LayersAndExtensions::RequestInstanceLayers(
      const std::vector<const char*>& layers) {

    uint32_t layerCount;
    vkEnumerateInstanceLayerProperties(&layerCount, nullptr);
    if(layerCount == 0 || layerCount < layers.size()) {
        ErrorManager::Validate(ERROR_TYPE_WARNING, "Layer not present"
            , "Instance layer request");
        return VK_ERROR_LAYER_NOT_PRESENT;
    }

    VkLayerProperties pAvailableLayers[layerCount];
    vkEnumerateInstanceLayerProperties(&layerCount, pAvailableLayers);

    for(size_t i = 0; i < layers.size(); i++) {
        int32_t layerIndex = LayerExist(layers[i], pAvailableLayers, layerCount);
        if(layerIndex == -1) {
            ErrorManager::Validate(ERROR_TYPE_WARNING, "Layer not present"
                , "Instance layer request");
            return VK_ERROR_LAYER_NOT_PRESENT;
        }
        enableInstanceLayers.push_back(pAvailableLayers[layerIndex]);
    }
    return VK_SUCCESS;
}

VkResult LayersAndExtensions::RequestInstanceExtensions(
    const std::vector<const char*>& requiredExtensions) {

    uint32_t extensionCount;
    vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
    if(extensionCount == 0 || extensionCount < requiredExtensions.size()) {
        ErrorManager::Validate(ERROR_TYPE_WARNING, "Extension not present"
            , "Instance extension request");
        return VK_ERROR_EXTENSION_NOT_PRESENT;
    }

    VkExtensionProperties availableExtensions[extensionCount];
    vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount
        , availableExtensions);

    for(size_t i = 0; i < requiredExtensions.size(); ++i) {
        int32_t extensionIndex = ExtensionExist(requiredExtensions[i]
            , availableExtensions, extensionCount);
        if(extensionIndex == -1) {
            std::cerr << "Require [ " << requiredExtensions[i] << " ] but "\
                "this extension not present\n";
            return VK_ERROR_EXTENSION_NOT_PRESENT;
        }
        enableInstanceExtensions.push_back(availableExtensions[extensionIndex]);
    }
    return VK_SUCCESS;
}

/*
VkResult LayersAndExtensions::RequestDeviceLayers(
      const std::vector<const char*>& layers, const VkPhysicalDevice& gpu) {

    uint32_t layerCount;
    vkEnumerateDeviceLayerProperties(gpu, &layerCount, nullptr);
    if(layerCount == 0 || layerCount < layers.size()) {
#ifdef DEBUG
        std::cerr << "\nERROR_TYPE_WARNING [ Device layer request ]\n---> "\
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
            std::cerr << "\nERROR_TYPE_WARNING [ Device layer request ]\n---> "\
                "Layer not present\n\n";
#endif
            return VK_ERROR_LAYER_NOT_PRESENT;
        }
        enableDeviceLayers.push_back(pAvailableLayers[layerIndex]);
    }
    return VK_SUCCESS;
}
*/

VkResult LayersAndExtensions::RequestDeviceExtensions(
      const std::vector<const char*>& extensions, const VkPhysicalDevice& gpu) {

    uint32_t extensionCount;
    vkEnumerateDeviceExtensionProperties(gpu, nullptr, &extensionCount, nullptr);
    if(extensionCount == 0 || extensionCount < extensions.size()) {
        ErrorManager::Validate(ERROR_TYPE_WARNING, "Extension not present"
            , "Device extension request");
        return VK_ERROR_EXTENSION_NOT_PRESENT;
    }

    VkExtensionProperties pAvailableExtensions[extensionCount];
    vkEnumerateDeviceExtensionProperties(gpu, nullptr, &extensionCount
        , pAvailableExtensions);

    for(size_t i = 0; i < extensions.size(); i++) {
        int32_t extensionIndex = ExtensionExist(extensions[i]
            , pAvailableExtensions, extensionCount);
        if(extensionIndex == -1) {
            ErrorManager::Validate(ERROR_TYPE_WARNING, "Extension not present"
                , "Device extension request");
            return VK_ERROR_EXTENSION_NOT_PRESENT;
        }
        enableDeviceExtensions.push_back(pAvailableExtensions[extensionIndex]);
    }
    return VK_SUCCESS;
}

const std::vector<VkLayerProperties>&
    LayersAndExtensions::GetInstanceLayers() const
{ return enableInstanceLayers; }

std::vector<const char*> LayersAndExtensions::GetInstanceLayerNames() const {
    std::vector<const char*> layerNames(enableInstanceLayers.size());
    for(size_t i = 0; i < layerNames.size(); i++) {
        layerNames[i] = enableInstanceLayers[i].layerName;
    }
    return layerNames;
}
    
const std::vector<VkExtensionProperties>&
    LayersAndExtensions::GetInstanceExtensions() const
{ return enableInstanceExtensions; }

std::vector<const char*> LayersAndExtensions::GetInstanceExtensionNames() const {
    std::vector<const char*> extensionNames(enableInstanceExtensions.size());
    for(size_t i = 0; i < extensionNames.size(); i++) {
        extensionNames[i] = enableInstanceExtensions[i].extensionName;
    }
    return extensionNames;
}
   
/*
const std::vector<VkLayerProperties>& 
    LayersAndExtensions::GetDeviceLayers() const
{ return enableDeviceLayers; }

std::vector<const char*> LayersAndExtensions::GetDeviceLayerNames() const {
    std::vector<const char*> layerNames(enableDeviceLayers.size());
    for(size_t i = 0; i < layerNames.size(); i++) {
        layerNames[i] = enableDeviceLayers[i].layerName;
    }
    return layerNames;
}
*/

const std::vector<VkExtensionProperties>&
    LayersAndExtensions::GetDeviceExtensions() const
{ return enableDeviceExtensions; }

std::vector<const char*> LayersAndExtensions::GetDeviceExtensionNames() const {
    std::vector<const char*> extensionNames(enableDeviceExtensions.size());
    for(size_t i = 0; i < extensionNames.size(); i++) {
        extensionNames[i] = enableDeviceExtensions[i].extensionName;
    }
    return extensionNames;
}

} //vk
