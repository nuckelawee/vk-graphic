#pragma once

#ifdef DEBUG

#include "vk_extensions.hpp"

class DebugMessenger {

    VkDebugUtilsMessengerEXT debugMessenger_;

public:

    void Setup(const VkInstance& instance);

    static VkDebugUtilsMessengerCreateInfoEXT PopulateDebugMessengerInfo();

    static VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(
          VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity
        , VkDebugUtilsMessageTypeFlagsEXT messageType
        , const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData
        , void* pUserData);

private:
    VkResult Create(const VkInstance& instance
        , VkDebugUtilsMessengerCreateInfoEXT *pCreateInfo
        , const VkAllocationCallbacks *pAllocator);

public:
    void Destroy(const VkInstance& instance
        , const VkAllocationCallbacks *pAllocator);

    DebugMessenger() {}
    ~DebugMessenger() {}
};

#endif
