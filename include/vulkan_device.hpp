#pragma once

#include <optional>
#include <functional>
#include <set>

#include "vk_extensions.hpp"

struct QueueFamily {
    std::optional<uint32_t> index;
    VkQueueFamilyProperties queueProperties;
       
    bool IsComplete() { return index.has_value(); }

    bool operator ==(const QueueFamily& queueFamily) const
    { return index == queueFamily.index; }
    bool operator <(const QueueFamily& queueFamily) const
    { return index < queueFamily.index; }
    bool operator >(const QueueFamily& queueFamily) const
    { return index > queueFamily.index; }
};

struct QueueFamilies {
    QueueFamily graphic;
    QueueFamily transfer;
    QueueFamily present;
};

class VulkanDevice {
    VkPhysicalDevice gpu = VK_NULL_HANDLE;
    VkDevice device;
    QueueFamilies queues;

public:

    VkResult CreateLogicalDevice(const VkInstance& instance
        , const VkSurfaceKHR& surface, const VulkanLayersAndExtensions& attachments);
    VkResult PickGpu(const VkInstance& instance, const VkSurfaceKHR& surface
        , VulkanLayersAndExtensions& attachments
        , std::function<unsigned int(const VkPhysicalDevice&, const VkSurfaceKHR&
        , VulkanLayersAndExtensions&)> IsGpuSuitable =
          ChooseDefaultGpu);

private:
    static VkResult FindQueueFamilies(QueueFamilies& indices
        , const VkPhysicalDevice& gpu, const VkSurfaceKHR& surface);
public: 
    static unsigned int ChooseDefaultGpu(const VkPhysicalDevice& gpu
        , const VkSurfaceKHR& surface
        , VulkanLayersAndExtensions& attachments);

#ifdef DEBUG
    static void PrintGpuProperties(const VkPhysicalDevice& gpu);
    static std::string GetGpuName(const VkPhysicalDevice& gpu);

private:
    static std::string GetGpuType(VkPhysicalDeviceType gpuType);

#endif
    std::vector<VkDeviceQueueCreateInfo> PopulateQueueInfos(
        const VkSurfaceKHR& surface, const float queuePriorities) const;

public:

    VkPhysicalDevice AccessGpu() { return gpu; }
    VkDevice Access() { return device; }
    QueueFamilies AccessQueues() { return queues; }

    void SetQueueFamilies(const VkSurfaceKHR& surface
        , std::function<VkResult(QueueFamilies& queueFamilies
        , const VkPhysicalDevice& gpu
        , const VkSurfaceKHR& surface)> find = FindQueueFamilies);

    void DestroyDevice();
    VulkanDevice() {}
    ~VulkanDevice() {}

};
