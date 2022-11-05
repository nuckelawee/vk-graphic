#include "vulkan_instance.hpp"

VkResult Vulkan_Instance::Create(VulkanLayersAndExtensions& attachments
    , const char* pAppName) {
    
    VkResult result;

    VkApplicationInfo applicationInfo {};
    applicationInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    applicationInfo.pApplicationName = pAppName;
    applicationInfo.applicationVersion = 
    applicationInfo.pEngineName = "No engine";
    applicationInfo.engineVersion = 0;
    applicationInfo.apiVersion = VK_API_VERSION_1_0;

    VkInstanceCreateInfo instanceInfo {};
    instanceInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    instanceInfo.pApplicationInfo = &applicationInfo;
    instanceInfo.enableLayerCount = attachments.GetInstanceLayers().size();
    instanceInfo.ppEnableLayerNames = attachments.GetInstanceLayers().data();
    instanceInfo.enableExtensionCount = attachments.GetInstanceExtensions().size();
    instanceInfo.ppEnableExtensionNames = attachments.GetInstanceExtensions().data();

    result = vkCreateInstance(&instanceInfo, nullptr, instance);
    assert(result == VK_SUCCESS);
    return result;
}

VulkanInstance::~VulkanInstance() {
    vkDestroyInstance(instance, nullptr);
}
