#include "vulkan_device.hpp"

#ifdef DEBUG
std::string VulkanDevice::GetGpuType(VkPhysicalDeviceType gpuType) {
    switch(gpuType) {
    case VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU:
        return "integrated GPU";
    case VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU:   
        return "discreate GPU";
    case VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU:
        return "virtual GPU";
    case VK_PHYSICAL_DEVICE_TYPE_CPU:
        return "CPU";
    default:
        return "other GPU";
    }
} 

std::string VulkanDevice::GetGpuName(const VkPhysicalDevice& gpu) {
    VkPhysicalDeviceProperties gpuProperties;
    vkGetPhysicalDeviceProperties(gpu, &gpuProperties);
    return gpuProperties.deviceName;
}

void VulkanDevice::PrintGpuProperties(const VkPhysicalDevice& gpu) {
    VkPhysicalDeviceProperties gpuProperties;
    vkGetPhysicalDeviceProperties(gpu, &gpuProperties);

    const std::string gpuType = GetGpuType(gpuProperties.deviceType);

    std::cout << "\n[GPU properties]" 
        << "\n\t[GPU name] ---> " << gpuProperties.deviceName
        << "\n\t[GPU type] ---> " << gpuType
        << "\n\t[API version] ---> " << gpuProperties.apiVersion
        << "\n\t[Driver version] ---> " << gpuProperties.driverVersion << '\n';
}

#endif

VkResult VulkanDevice::FindQueueFamilyIndices(QueueFamilyIndices& indices
    , const VkPhysicalDevice& gpu, const VkSurfaceKHR& surface) {

    uint32_t queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(gpu, &queueFamilyCount, nullptr);
    VkQueueFamilyProperties queueProperties[queueFamilyCount];
    vkGetPhysicalDeviceQueueFamilyProperties(gpu, &queueFamilyCount, queueProperties);

    for(size_t i = 0; i < queueFamilyCount; i++) {
        const VkQueueFamilyProperties& queueProperty = queueProperties[i];
        if( (queueProperty.queueFlags & VK_QUEUE_TRANSFER_BIT)
            && !(queueProperty.queueFlags & VK_QUEUE_GRAPHICS_BIT)) {
            indices.transfer = i;
            continue;
        }
        if(queueProperty.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
            indices.graphic = i;
        }
        VkBool32 isPresentSupport = false;
        vkGetPhysicalDeviceSurfaceSupportKHR(gpu, i, surface, &isPresentSupport);
        if(isPresentSupport == true) {
            indices.present = i;
        }
    }

    if(indices.present.has_value() && indices.graphic.has_value()) {
        if(indices.transfer.has_value())
        { return VK_SUCCESS; }
#ifdef DEBUG
        std::cerr << "\nWARNING [ " << GetGpuName(gpu)
            << " ]\n---> Transfer department family indices not available\n\n";
#endif
        return VK_INCOMPLETE;
    }
#ifdef DEBUG
    std::cerr << "\nWARNING [ " << GetGpuName(gpu) 
        << " ]\n---> Family indices not supported\n\n";
#endif
    return VK_ERROR_FEATURE_NOT_PRESENT;
}

bool VulkanDevice::ChooseDefaultGpu(const VkPhysicalDevice& gpu
    , const VkSurfaceKHR& surface, VulkanLayersAndExtensions& attachments) {

#ifdef DEBUG
    PrintGpuProperties(gpu);
#endif
    QueueFamilyIndices indices;
    if(FindQueueFamilyIndices(indices, gpu, surface) != VK_SUCCESS) {
#ifdef DEBUG
        std::cerr << "\nWARNING [ " << GetGpuName(gpu)
            << " ]\n---> Not all GPU family indices supported\n\n";
#endif
        return false;
    }

    const std::vector<const char*> extensions =
        { VK_KHR_SWAPCHAIN_EXTENSION_NAME };
    if(attachments.RequestDeviceExtensions(extensions, gpu) != VK_SUCCESS) {
#ifdef DEBUG
        std::cerr << "\nWARNING [ " << GetGpuName(gpu)
            << " ]\n---> GPU extensions doesn't supported\n\n";
#endif
        return false;
    }
    return true; 
}

VkResult VulkanDevice::PickGpu(const VkInstance& instance
    , const VkSurfaceKHR& surface, VulkanLayersAndExtensions& attachments) {
//    , std::function<bool>(VkPhysicalDevice) isGpuSuitable = VulkanDevice::DefaultGpu() {
    uint32_t deviceCount = 0;
    vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);
#ifdef DEBUG
    if(deviceCount == 0) {
        std::cerr << "\nERROR [ pick GPU ]\n---> Failed to find any GPU\n\n";
    }
    assert(deviceCount != 0);
#endif
    VkPhysicalDevice pGpus[deviceCount];
    vkEnumeratePhysicalDevices(instance, &deviceCount, pGpus);
    for(size_t i = 0; i < deviceCount; i++) {
        if(ChooseDefaultGpu(pGpus[i], surface, attachments)) {
            gpu = pGpus[i];
            break;
        }
    }
    if(gpu == VK_NULL_HANDLE) {
#ifdef DEBUG
        std::cerr << "\nERROR [ Pick GPU ]\n---> "\
            "suitable not found GPU\n";
        exit(EXIT_FAILURE);
#endif
        return VK_ERROR_INITIALIZATION_FAILED;
    }
    return VK_SUCCESS;
}
    
void VulkanDevice::CreateLogicalDevice(const VkInstance& instance) {
    
}

VulkanDevice::~VulkanDevice() {

}
