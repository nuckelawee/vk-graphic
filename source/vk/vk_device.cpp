#include "vk/vk_device.hpp"

namespace vk {

unsigned int queueFamilyGraphicPriority(VkQueueFlags provided
    , const VkPhysicalDevice& gpu, const VkSurfaceKHR& surface
    , uint32_t index) {

    if(provided & VK_QUEUE_GRAPHICS_BIT) {
        VkBool32 isPresentSupport = false;
        vkGetPhysicalDeviceSurfaceSupportKHR(gpu, index
            , surface, &isPresentSupport);
        if((!isPresentSupport && !(provided & VK_QUEUE_TRANSFER_BIT))) { 
            return 3;
        }
        if((!isPresentSupport || !(provided & VK_QUEUE_TRANSFER_BIT))) { 
            return 2;
        }
        return 1;
    }
    return 0;
}

unsigned int queueFamilyTransferPriority(VkQueueFlags provided
    , const VkPhysicalDevice& gpu,  const VkSurfaceKHR& surface
    , uint32_t index) {
    
    if(provided & VK_QUEUE_TRANSFER_BIT) {
        VkBool32 isPresentSupport = false;
        vkGetPhysicalDeviceSurfaceSupportKHR(gpu, index
            , surface, &isPresentSupport);
        if((!isPresentSupport && !(provided & VK_QUEUE_GRAPHICS_BIT))) {
            return 3;
        }
        if((!isPresentSupport || !(provided & VK_QUEUE_GRAPHICS_BIT))) {
            return 2;
        }
        return 1;
    }
    return 0;
}

unsigned int queueFamilyPresentPriority(VkQueueFlags provided
    , const VkPhysicalDevice& gpu, const VkSurfaceKHR& surface
    , uint32_t index) {

    VkBool32 isPresentSupport = false;
    vkGetPhysicalDeviceSurfaceSupportKHR(gpu, index
        , surface, &isPresentSupport);
    if(isPresentSupport) {
        if(!(provided & VK_QUEUE_TRANSFER_BIT) && !(provided & VK_QUEUE_GRAPHICS_BIT)) {
            return 3;
        }
        if(!(provided & VK_QUEUE_TRANSFER_BIT) || !(provided & VK_QUEUE_GRAPHICS_BIT)) {
            return 2;
        }
        return 1;
    }
    return 0;
}

#ifdef DEBUG
std::string Device::GetGpuType(VkPhysicalDeviceType gpuType) {
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

std::string Device::GetGpuName(const VkPhysicalDevice& gpu) {
    VkPhysicalDeviceProperties gpuProperties;
    vkGetPhysicalDeviceProperties(gpu, &gpuProperties);
    return gpuProperties.deviceName;
}

void Device::PrintGpuProperties(const VkPhysicalDevice& gpu) {
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

VkResult Device::FindQueueFamilies(QueueFamilies& queueFamilies
    , const VkPhysicalDevice& gpu, const Surface& surface
    , void *pUserData) {

    uint32_t queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(gpu, &queueFamilyCount, nullptr);
    VkQueueFamilyProperties queueProperties[queueFamilyCount];
    vkGetPhysicalDeviceQueueFamilyProperties(gpu, &queueFamilyCount, queueProperties);

    unsigned int graphicPriority = 0;
    unsigned int transferPriority = 0;
    unsigned int presentPriority = 0;
    for(size_t i = 0; i < queueFamilyCount; i++) {
        const VkQueueFamilyProperties& queueProperty = queueProperties[i];
        if(graphicPriority < queueFamilyGraphicPriority(queueProperty.queueFlags
            , gpu, surface.Access(), i)) {

            graphicPriority = queueFamilyGraphicPriority(queueProperty.queueFlags
                , gpu, surface.Access(), i);
            queueFamilies.graphic.index = i;
            queueFamilies.graphic.queueProperties = queueProperty;
        }
        if(transferPriority < queueFamilyTransferPriority(queueProperty.queueFlags
            , gpu, surface.Access(), i)) {

            transferPriority = queueFamilyTransferPriority(queueProperty.queueFlags
                , gpu, surface.Access(), i);
            queueFamilies.transfer.index = i;
            queueFamilies.transfer.queueProperties = queueProperty;
        }
        if(presentPriority < queueFamilyPresentPriority(queueProperty.queueFlags
            , gpu, surface.Access(), i)) {

            presentPriority = queueFamilyPresentPriority(queueProperty.queueFlags
                , gpu, surface.Access(), i);
            queueFamilies.present.index = i;
            queueFamilies.present.queueProperties = queueProperty;
        }
    }

    if(graphicPriority == 2 && presentPriority == 2 && transferPriority == 2) {
        return VK_SUCCESS;
    } 
    if(graphicPriority == 0 || presentPriority == 0 || transferPriority == 0) {
        ErrorManager::Validate(ERROR_TYPE_WARNING, "Not all queue families supported"
            , GetGpuName(gpu));
        return VK_ERROR_FEATURE_NOT_PRESENT;
    }
    ErrorManager::Validate(ERROR_TYPE_WARNING, "Queue capabilities combined", GetGpuName(gpu));
    return VK_INCOMPLETE;
}

unsigned int Device::ChooseDefaultGpu(const VkPhysicalDevice& gpu
    , const Surface& surface, LayersAndExtensions& attachments) {

    unsigned int priority = 0;
#ifdef DEBUG
    PrintGpuProperties(gpu);
#endif
    QueueFamilies queueFamilies;
    VkResult result = FindQueueFamilies(queueFamilies, gpu, surface, nullptr);
    if(result == VK_ERROR_FEATURE_NOT_PRESENT) {
        return priority;
    }
    if(result == VK_INCOMPLETE) {
        priority++;
    } else {
        priority += 2;
    }

    const std::vector<const char*> extensions =
        { VK_KHR_SWAPCHAIN_EXTENSION_NAME };
    if(attachments.RequestDeviceExtensions(extensions, gpu) != VK_SUCCESS) {
        ErrorManager::Validate(ERROR_TYPE_WARNING, "GPU extensions doesn't supported"
            , GetGpuName(gpu));
        return 0;
    }

    SurfaceDetails surfaceCapabilities = surface.Capabilities(gpu);
    if(!(surfaceCapabilities.formats.size() > 0)
        || !(surfaceCapabilities.presentModes.size() > 0)) {
        ErrorManager::Validate(ERROR_TYPE_WARNING, "GPU surface doesn't supported"
            , GetGpuName(gpu));
        return 0; 
    }

    VkPhysicalDeviceProperties gpuProperties;
    vkGetPhysicalDeviceProperties(gpu, &gpuProperties);
 
    if(gpuProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) {
        priority += 2;
    }
    return priority; 
}

void Device::PickGpu(const Instance& instance
    , const Surface& surface, LayersAndExtensions& attachments
    , std::function<unsigned int(const VkPhysicalDevice&, const Surface&
    , LayersAndExtensions&)> IsGpuSuitable) {

    uint32_t deviceCount = 0;
    vkEnumeratePhysicalDevices(instance.Access(), &deviceCount, nullptr);
    ErrorManager::Validate(Error(ERROR_TYPE_WARNING, deviceCount == 0), "Failed to find any GPU"
        , "Pick GPU");
    VkPhysicalDevice pGpus[deviceCount];
    vkEnumeratePhysicalDevices(instance.Access(), &deviceCount, pGpus);
    unsigned int suitable = 0;
    for(size_t i = 0; i < deviceCount; i++) {
        unsigned int priority = IsGpuSuitable(pGpus[i], surface, attachments);
        if(priority > suitable) {
            suitable = priority;        
            gpu_ = pGpus[i];
        }
    }
    ErrorManager::Validate(Error(ERROR_TYPE_UNSOLVABLE, gpu_ == VK_NULL_HANDLE)
        , "suitable GPU not found", "Pick GPU");
}

int32_t Device::FindMemoryProperties(uint32_t memoryTypeBits
    , VkMemoryPropertyFlags requiredProperties) const {

    VkPhysicalDeviceMemoryProperties memoryProperties;
    vkGetPhysicalDeviceMemoryProperties(gpu_, &memoryProperties);
      
    for(uint32_t i = 0; i < memoryProperties.memoryTypeCount; i++) {
        const bool isRequiredMemoryType = requiredProperties & (1 << i);
        const bool hasRequiredProperties 
            = (memoryProperties.memoryTypes[i].propertyFlags & requiredProperties)
            == requiredProperties;
        if(isRequiredMemoryType && hasRequiredProperties) {
            return i;
        }
    }
    return -1;
}
/*
std::vector<VkDeviceQueueCreateInfo> Device::PopulateQueueInfos(
    const Surface& surface, const float *pQueuePriorities) const {

   return queueInfos;
}
*/

void Device::CreateLogicalDevice(const Surface& surface
    , const LayersAndExtensions& attachments) {

    QueueFamilies queueFamilies;
    FindQueueFamilies(queueFamilies, gpu_, surface, nullptr);
    std::set<QueueFamily> uniqueQueueFamilies = {
        queueFamilies.graphic,
        queueFamilies.present,
        queueFamilies.transfer,
    };
    uint32_t maxQueueCount = std::max( {
          queueFamilies.graphic.queueProperties.queueCount
        , queueFamilies.present.queueProperties.queueCount
        , queueFamilies.transfer.queueProperties.queueCount
    } );

    std::vector<VkDeviceQueueCreateInfo> queueInfos;
    VkDeviceQueueCreateInfo queueInfo {};
    float pQueuePriorities[maxQueueCount] = { 1.0f };
    for(auto& queueFamily : uniqueQueueFamilies) {
        queueInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queueInfo.queueFamilyIndex = queueFamily.index.value();
        queueInfo.queueCount = queueFamily.queueProperties.queueCount;
        queueInfo.pQueuePriorities = pQueuePriorities;
        queueInfos.push_back(queueInfo);
    }

    VkPhysicalDeviceFeatures deviceFeatures {};
    deviceFeatures.samplerAnisotropy = VK_TRUE;
    std::vector<const char*> deviceExtensions = attachments.GetDeviceExtensionNames();

    VkDeviceCreateInfo deviceInfo {};
    deviceInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    deviceInfo.queueCreateInfoCount = queueInfos.size();
    deviceInfo.pQueueCreateInfos = queueInfos.data();
    deviceInfo.enabledExtensionCount = deviceExtensions.size();
    deviceInfo.ppEnabledExtensionNames = deviceExtensions.data();
    deviceInfo.pEnabledFeatures = &deviceFeatures;

    VkResult result = vkCreateDevice(gpu_, &deviceInfo, nullptr, &device_);
    ErrorManager::Validate(result, "Logical device creation");

    vkGetDeviceQueue(device_, queues_.graphic.index.value(), 0
        , &(queues_.graphic.queue));
    vkGetDeviceQueue(device_, queues_.present.index.value(), 0
        , &(queues_.present.queue));
    vkGetDeviceQueue(device_, queues_.transfer.index.value(), 0
        , &(queues_.transfer.queue));
}

VkFormat Device::FindSupportedFormat(const std::vector<VkFormat>& candidates
    , VkImageTiling tiling, VkFormatFeatureFlags features) const {

    for(VkFormat format : candidates) {
        VkFormatProperties properties;
        vkGetPhysicalDeviceFormatProperties(gpu_, format, &properties);
        if(tiling == VK_IMAGE_TILING_LINEAR && ((properties.linearTilingFeatures
             & features) == features)) {
            return format;
        } else
        if(tiling == VK_IMAGE_TILING_OPTIMAL && ((properties.optimalTilingFeatures
             & features) == features)) {
            return format;
        }
    }
    ErrorManager::Validate(ERROR_TYPE_UNSOLVABLE, "Failed to find supported format"
        , "GPU");
    return VK_FORMAT_UNDEFINED;
}

void Device::SetQueueFamilies(const Surface& surface
    , std::function<VkResult(QueueFamilies& queueFamilies
    , const VkPhysicalDevice& gpu
    , const Surface& surface
    , void *pUserData)> find) {
    
    find(queues_, gpu_, surface, nullptr); 
#ifdef DEBUG
    std::cout << "Queue family indices:\n\tgraphic queue --->  [ " 
        << queues_.graphic.index.value() << " ]\n\tpresent queue --->  [ "
        << queues_.present.index.value() << " ]\n\ttransfer queue ---> [ "
        << queues_.transfer.index.value() << " ]\n\n";
#endif
}

void Device::Destroy() {
    vkDestroyDevice(device_, nullptr);
}

} //vk
