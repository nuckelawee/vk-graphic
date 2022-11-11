#include "vulkan_device.hpp"

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

VkResult VulkanDevice::FindQueueFamilies(QueueFamilies& queueFamilies
    , const VkPhysicalDevice& gpu, const VkSurfaceKHR& surface) {

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
            , gpu, surface, i)) {

            graphicPriority = queueFamilyGraphicPriority(queueProperty.queueFlags
                , gpu, surface, i);
            queueFamilies.graphic.index = i;
            queueFamilies.graphic.queueProperties = queueProperty;
        }
        if(transferPriority < queueFamilyTransferPriority(queueProperty.queueFlags
            , gpu, surface, i)) {

            transferPriority = queueFamilyTransferPriority(queueProperty.queueFlags
                , gpu, surface, i);
            queueFamilies.transfer.index = i;
            queueFamilies.transfer.queueProperties = queueProperty;
        }
        if(presentPriority < queueFamilyPresentPriority(queueProperty.queueFlags
            , gpu, surface, i)) {

            presentPriority = queueFamilyPresentPriority(queueProperty.queueFlags
                , gpu, surface, i);
            queueFamilies.present.index = i;
            queueFamilies.present.queueProperties = queueProperty;
        }
    }

    if(graphicPriority == 2 && presentPriority == 2 && transferPriority == 2) {
        return VK_SUCCESS;
    } 
    if(graphicPriority == 0 || presentPriority == 0 || transferPriority == 0) {
#ifdef DEBUG
        std::cerr << "\nWARNING [ " << GetGpuName(gpu) 
            << " ]\n---> Not all queue families supported\n\n";
#endif   
        return VK_ERROR_FEATURE_NOT_PRESENT;
    }
#ifdef DEBUG
    std::cerr << "\nWARNING [ " << GetGpuName(gpu)
        << " ]\n---> Queue capabilities combined \n\n";
#endif
    return VK_INCOMPLETE;
}

unsigned int VulkanDevice::ChooseDefaultGpu(const VkPhysicalDevice& gpu
    , const VkSurfaceKHR& surface, VulkanLayersAndExtensions& attachments) {

    unsigned int priority = 0;
#ifdef DEBUG
    PrintGpuProperties(gpu);
#endif
    QueueFamilies queueFamilies;
    VkResult result = FindQueueFamilies(queueFamilies, gpu, surface);
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
#ifdef DEBUG
        std::cerr << "\nWARNING [ " << GetGpuName(gpu)
            << " ]\n---> GPU extensions doesn't supported\n\n";
#endif
        return 0;
    }

    SurfaceDetails surfaceCapabilities = Window::SurfaceCapabilities(gpu
        , surface);
    if(!(surfaceCapabilities.formats.size() > 0)
        || !(surfaceCapabilities.presentModes.size() > 0)) {
#ifdef DEBUG
        std::cerr << "\nWARNING [ " << GetGpuName(gpu)
            << " ]\n---> GPU surface doesn't supported\n\n"; 
#endif
        return 0; 
    }

    VkPhysicalDeviceProperties gpuProperties;
    vkGetPhysicalDeviceProperties(gpu, &gpuProperties);
 
    if(gpuProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) {
        priority += 2;
    }
    return priority; 
}

VkResult VulkanDevice::PickGpu(const VkInstance& instance
    , const VkSurfaceKHR& surface, VulkanLayersAndExtensions& attachments
    , std::function<unsigned int(const VkPhysicalDevice&, const VkSurfaceKHR&
    , VulkanLayersAndExtensions&)> IsGpuSuitable) {

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
    unsigned int suitable = 0;
    for(size_t i = 0; i < deviceCount; i++) {
        unsigned int priority = IsGpuSuitable(pGpus[i], surface, attachments);
        if(priority > suitable) {
            suitable = priority;        
            gpu_ = pGpus[i];
        }
    }
    if(gpu_ == VK_NULL_HANDLE) {
#ifdef DEBUG
        std::cerr << "\nERROR [ Pick GPU ]\n---> "\
            "suitable not found GPU\n";
        exit(EXIT_FAILURE);
#endif
        return VK_ERROR_INITIALIZATION_FAILED;
    }
    FindQueueFamilies(queues_, gpu_, surface);
    return VK_SUCCESS;
}

std::vector<VkDeviceQueueCreateInfo> VulkanDevice::PopulateQueueInfos(
    const VkSurfaceKHR& surface, const float& queuePriorities) const {

    QueueFamilies queueFamilies;
    FindQueueFamilies(queueFamilies, gpu_, surface);
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
        queueInfo.queueCount = queueFamily.queueProperties.queueCount;
        queueInfo.pQueuePriorities = &queuePriorities;
        queueInfos.push_back(queueInfo);
    }
    return queueInfos;
}

#ifdef DEBUG
void ProccessInstanceErrorCreation(VkResult result) {
    switch(result) {
    case VK_ERROR_OUT_OF_HOST_MEMORY:
        std::cerr << "\nERROR [ Instance error creation ]\n---> "\
            "Error out of host memory\n\n";
    case VK_ERROR_OUT_OF_DEVICE_MEMORY:
        std::cerr << "\nERROR [ Instance error creation ]\n---> "\
            "Error out of device memory\n\n";
    case VK_ERROR_INITIALIZATION_FAILED:
        std::cerr << "\nERROR [ Instance error creation ]\n---> "\
            "Error initialization\n\n";
    case VK_ERROR_EXTENSION_NOT_PRESENT:
        std::cerr << "\nERROR [ Instance error creation ]\n---> "\
            "Error extension not present\n\n";
    case VK_ERROR_FEATURE_NOT_PRESENT:
        std::cerr << "\nERROR [ Instance error creation ]\n---> "\
            "Error feature not present\n\n";
    case VK_ERROR_TOO_MANY_OBJECTS:
        std::cerr << "\nERROR [ Instance error creation ]\n---> "\
            "Error too many objects\n\n";
    case VK_ERROR_DEVICE_LOST:
        std::cerr << "\nERROR [ Instance error creation ]\n---> "\
            "Error device lost\n\n";
    default:
        std::cerr << "\nERROR [ Instance error creation ]\n---> "\
            "Unknown error\n\n";
    }
}
#endif
    
VkResult VulkanDevice::CreateLogicalDevice(const VkInstance& instance
    , const VkSurfaceKHR& surface, const VulkanLayersAndExtensions& attachments) {

    const float queuePriorities = 1.0f;
    std::vector<VkDeviceQueueCreateInfo> queueInfos = PopulateQueueInfos(surface
        , queuePriorities);

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
#ifdef DEBUG
    if(result != VK_SUCCESS) {
        ProccessInstanceErrorCreation(result);
        std::cerr << "\nERROR [ Device creation ]\n---> "\
            "Failed to create device\n";
    }
#endif
    assert(result == VK_SUCCESS);
    return result;
}

void VulkanDevice::SetQueueFamilies(const VkSurfaceKHR& surface
    , std::function<VkResult(QueueFamilies& queueFamilies
    , const VkPhysicalDevice& gpu
    , const VkSurfaceKHR& surface)> find) {
    
    find(queues_, gpu_, surface); 
}

void VulkanDevice::DestroyDevice() {
    vkDestroyDevice(device_, nullptr);
}
