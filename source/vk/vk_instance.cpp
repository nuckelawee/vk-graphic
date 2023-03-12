#include "vk/vk_instance.hpp"

namespace vk {

void Instance::Create(const AppSetting& setting,
    const LayersAndExtensions& attachments) {
    
    VkResult result;

    VkApplicationInfo applicationInfo {};
    applicationInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    applicationInfo.pApplicationName = setting.Application().c_str();
    applicationInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    applicationInfo.pEngineName = setting.Vulkan().Engine().c_str();
    applicationInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    applicationInfo.apiVersion = VK_API_VERSION_1_0;

    std::vector<const char*> extensionNames = attachments.GetInstanceExtensionNames();
    VkInstanceCreateInfo instanceInfo {};
    instanceInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    instanceInfo.pApplicationInfo = &applicationInfo;
    instanceInfo.enabledExtensionCount = attachments.GetInstanceExtensions().size();
    instanceInfo.ppEnabledExtensionNames = extensionNames.data();

    instanceInfo.enabledLayerCount = 0;
    instanceInfo.ppEnabledLayerNames = nullptr;
#ifdef DEBUG
    std::vector<const char*> layerNames = attachments.GetInstanceLayerNames();
    instanceInfo.enabledLayerCount = attachments.GetInstanceLayers().size();
    instanceInfo.ppEnabledLayerNames = layerNames.data();
    VkDebugUtilsMessengerCreateInfoEXT debugMessengerInfo = 
        DebugMessenger::PopulateDebugMessengerInfo();
    instanceInfo.pNext = &debugMessengerInfo;
#endif
    result = vkCreateInstance(&instanceInfo, nullptr, &instance_);
    ErrorManager::Validate(result, "Instance creation");
    debugMessenger_.Setup(instance_);
}

void Instance::IncludeDefaultLayersAndExtensions(
    LayersAndExtensions& attachments) {

    uint32_t glfwExtensionCount = 0;
    const char** ppGlfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

    std::vector<const char*> glfwExtensions(ppGlfwExtensions
        , ppGlfwExtensions + glfwExtensionCount);

    glfwExtensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    VkResult result = attachments.RequestInstanceExtensions(glfwExtensions);
    ErrorManager::Validate(Error(ERROR_TYPE_UNSOLVABLE, result != VK_SUCCESS)
        , "GLFW extensions doesn't support", "Include layers and extensions");

#ifdef DEBUG
    const std::vector<const char*> validationLayer =
    { "VK_LAYER_KHRONOS_validation" };

    result = attachments.RequestInstanceLayers(validationLayer);
    ErrorManager::Validate(Error(ERROR_TYPE_UNSOLVABLE, result != VK_SUCCESS)
        , "Validation layer doesn't active", "Include layers and extensions");
#endif
}

Instance::~Instance() {
    debugMessenger_.Destroy(instance_, nullptr);
    vkDestroyInstance(instance_, nullptr);
}

} //vk
