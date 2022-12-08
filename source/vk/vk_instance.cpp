#include "vk/vk_instance.hpp"

namespace vk {

VkResult Instance::Create(const LayersAndExtensions& attachments
    , const AppSetting& appSetting) {
    
    VkResult result;

    VkApplicationInfo applicationInfo {};
    applicationInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    applicationInfo.pApplicationName = appSetting.AppName().c_str();
    applicationInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    applicationInfo.pEngineName = "No engine";
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

#ifdef DEBUG
    if(result != VK_SUCCESS) {
        std::cerr << "\nERROR [ Instance creation ]\n---> "\
            "Failed to create instance\n\n";
    }
    assert(result == VK_SUCCESS);
    debugMessenger_.Setup(instance_);
#endif

    return result;
}

VkResult Instance::IncludeDefaultLayersAndExtensions(
    LayersAndExtensions& attachments) {

    uint32_t glfwExtensionCount = 0;
    const char** ppGlfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

    std::vector<const char*> glfwExtensions(ppGlfwExtensions
        , ppGlfwExtensions + glfwExtensionCount);

#ifdef DEBUG
    glfwExtensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
#endif
    VkResult result = attachments.RequestInstanceExtensions(glfwExtensions);
#ifdef DEBUG
    if(result != VK_SUCCESS) {
        std::cerr << "\nERROR [ GLFW extensions ]\n---> "\
            "GLFW extensions doesn't support\n\n";
    }
    assert(result == VK_SUCCESS);

#else
    return result;
#endif
#ifdef DEBUG
    const std::vector<const char*> validationLayer =
    { "VK_LAYER_KHRONOS_validation" };

    result = attachments.RequestInstanceLayers(validationLayer);
    if(result != VK_SUCCESS) {
        std::cerr << "\nWARNING [ validation layer ]\n---> "\
            "Validation layer doesn't active\n\n";
    }
    return result;
#endif
}

Instance::~Instance() {
    debugMessenger_.Destroy(instance_, nullptr);
    vkDestroyInstance(instance_, nullptr);
}

} //vk
