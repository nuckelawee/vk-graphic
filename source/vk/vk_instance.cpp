#include "vk/vk_instance.hpp"
#include "vk/vk_settings.hpp"
#include "vk/vk_exception.hpp"
#include "global_settings.hpp"

namespace vk {

void Instance::Create(const LayersAndExtensions& attachments) {
    
    VkApplicationInfo applicationInfo {};
    applicationInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    applicationInfo.pApplicationName = GlobalSettings::GetInstance().applicationName;
    applicationInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    applicationInfo.pEngineName = vk::Settings::GetInstance().engineName;
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
    VkResult result = vkCreateInstance(&instanceInfo, nullptr, &instance_);
    if(result != VK_SUCCESS) {
        throw Exception("Failed to create instance", result);
    } 
    debugMessenger_.Setup(instance_);
}

void Instance::IncludeDefaultLayersAndExtensions(
    LayersAndExtensions& attachments) {

    uint32_t glfwExtensionCount = 0;
    const char** glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

    std::vector<const char*> requestingExtensions(glfwExtensions
        , glfwExtensions + glfwExtensionCount);
    requestingExtensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    VkResult result = attachments.RequestInstanceExtensions(requestingExtensions);
    if(result != VK_SUCCESS) {
        throw Exception("GLFW extensions doesn't support", result); 
    }

#ifdef DEBUG
    const std::vector<const char*> validationLayer =
    { "VK_LAYER_KHRONOS_validation" };

    result = attachments.RequestInstanceLayers(validationLayer);
    if(result != VK_SUCCESS) {
        throw Exception("Validation layer doesn't active", result);
    }
#endif
}

VkInstance Instance::Access() const noexcept { return instance_; }

Instance::~Instance() {
    debugMessenger_.Destroy(instance_, nullptr);
    vkDestroyInstance(instance_, nullptr);
}

} //vk
