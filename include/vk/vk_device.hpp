#pragma once

#include <optional>
#include <functional>
#include <set>

#include "vk_surface.hpp"
#include "vk_extensions.hpp"
#include "vk_instance.hpp"

namespace vk {

struct QueueFamily {
    std::optional<uint32_t> index;
    VkQueueFamilyProperties queueProperties;
    VkQueue queue;
       
    bool IsComplete() { return index.has_value(); }

    bool operator ==(const QueueFamily& queueFamily) const
    { return index == queueFamily.index; }
    bool operator !=(const QueueFamily& queueFamily) const
    { return index != queueFamily.index; }
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

class Device {
    VkPhysicalDevice gpu_ = VK_NULL_HANDLE;
    VkDevice device_;
    QueueFamilies queues_;

public:

    void CreateLogicalDevice(const Surface& surface
        , const LayersAndExtensions& attachments);
    void PickGpu(const Instance& instance, const Surface& surface
        , LayersAndExtensions& attachments
        , std::function<unsigned int(const VkPhysicalDevice&, const Surface&
        , LayersAndExtensions&)> IsGpuSuitable =
          ChooseDefaultGpu);

    int32_t FindMemoryProperties(uint32_t memoryTypeBits
        , VkMemoryPropertyFlags requiredProperties) const;

private:
    static VkResult FindQueueFamilies(QueueFamilies& indices
        , const VkPhysicalDevice& gpu, const Surface& surface
        , void *pUserData);
public: 
    static unsigned int ChooseDefaultGpu(const VkPhysicalDevice& gpu
        , const Surface& surface
        , LayersAndExtensions& attachments);

#ifdef DEBUG
    static void PrintGpuProperties(const VkPhysicalDevice& gpu);
    static std::string GetGpuName(const VkPhysicalDevice& gpu);

private:
    static std::string GetGpuType(VkPhysicalDeviceType gpuType);

#endif

public:

    VkPhysicalDevice AccessGpu() const { return gpu_; }
    VkDevice Access() const { return device_; }
    QueueFamilies AccessQueues() const { return queues_; }

    VkFormat FindSupportedFormat(const std::vector<VkFormat>& candidates,
        VkImageTiling tiling, VkFormatFeatureFlags features) const;

    void SetQueueFamilies(const Surface& surface
        , std::function<VkResult(QueueFamilies& queueFamilies
        , const VkPhysicalDevice& gpu
        , const Surface& surface
        , void *pUserData)> find = FindQueueFamilies);

    void Destroy();
    Device() {}
    ~Device() {}

};

} //vk
