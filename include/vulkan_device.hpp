#pragma once

#include <optional>

#include "vk_extensions.hpp"

struct QueueFamilyIndices {
    std::optional<uint32_t> graphic;
    std::optional<uint32_t> present;
    std::optional<uint32_t> transfer;
};

class VulkanDevice {
    VkPhysicalDevice gpu = VK_NULL_HANDLE;
    VkDevice device;

public:

    void CreateLogicalDevice(const VkInstance& instance);
    VkResult PickGpu(const VkInstance& instance, const VkSurfaceKHR& surface
        , VulkanLayersAndExtensions& attachments);

    VkPhysicalDevice AccessGpu() { return gpu; }
    VkDevice Access() { return device; }

    static VkResult FindQueueFamilyIndices(QueueFamilyIndices& indices
        , const VkPhysicalDevice& gpu, const VkSurfaceKHR& surface);
    static bool ChooseDefaultGpu(const VkPhysicalDevice& gpu
        , const VkSurfaceKHR& surface
        , VulkanLayersAndExtensions& attachments);

#ifdef DEBUG
    static void PrintGpuProperties(const VkPhysicalDevice& gpu);
    static std::string GetGpuName(const VkPhysicalDevice& gpu);

private:
    static std::string GetGpuType(VkPhysicalDeviceType gpuType);

#endif
public:

    VulkanDevice() {}
    ~VulkanDevice();

};
