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
        ErrorManager::Validate(WARNING, "Not all queue families supported"
            , GetGpuName(gpu));
        return VK_ERROR_FEATURE_NOT_PRESENT;
    }
    ErrorManager::Validate(WARNING, "Queue capabilities combined", GetGpuName(gpu));
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
        ErrorManager::Validate(WARNING, "GPU extensions doesn't supported"
            , GetGpuName(gpu));
        return 0;
    }

    SurfaceDetails surfaceCapabilities = surface.Capabilities(gpu);
    if(!(surfaceCapabilities.formats.size() > 0)
        || !(surfaceCapabilities.presentModes.size() > 0)) {
        ErrorManager::Validate(WARNING, "GPU surface doesn't supported"
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
    ErrorManager::Validate(Error(UNSOLVABLE, deviceCount == 0), "Failed to find any GPU"
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
    ErrorManager::Validate(Error(UNSOLVABLE, gpu_ == VK_NULL_HANDLE)
        , "suitable GPU not found", "Pick GPU");
    //FindQueueFamilies(queues_, gpu_, surface, nullptr);
}

std::vector<VkDeviceQueueCreateInfo> Device::PopulateQueueInfos(
    const Surface& surface, const float *pQueuePriorities) const {

    QueueFamilies queueFamilies;
    FindQueueFamilies(queueFamilies, gpu_, surface, nullptr);
    std::set<QueueFamily> uniqueQueueFamilies = {
        queueFamilies.graphic,
        queueFamilies.present,
        queueFamilies.transfer,
    };

    std::vector<VkDeviceQueueCreateInfo> queueInfos;
    VkDeviceQueueCreateInfo queueInfo {};
    for(auto& queueFamily : uniqueQueueFamilies) {
        queueInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queueInfo.queueFamilyIndex = queueFamily.index.value();
        queueInfo.queueCount = 1;//queueFamily.queueProperties.queueCount;
        queueInfo.pQueuePriorities = pQueuePriorities;
        queueInfos.push_back(queueInfo);
    }
    return queueInfos;
}

void Device::CreateLogicalDevice(const Surface& surface
    , const LayersAndExtensions& attachments) {

    const float queuePriorities = 1.0f;
    std::vector<VkDeviceQueueCreateInfo> queueInfos = PopulateQueueInfos(surface
        , &queuePriorities);

    VkPhysicalDeviceFeatures deviceFeatures {};
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
}

void Device::SetQueueFamilies(const Surface& surface
    , std::function<VkResult(QueueFamilies& queueFamilies
    , const VkPhysicalDevice& gpu
    , const Surface& surface
    , void *pUserData)> find) {
    
    find(queues_, gpu_, surface, nullptr); 
}

void Device::Destroy() {
    vkDestroyDevice(device_, nullptr);
}

} //vk
