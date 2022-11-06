#pragma once

#include "vk_extensions.hpp"
#include "app_setting.hpp"
#include "debug_messenger.hpp"

class VulkanInstance {

    VkInstance instance;
    DebugMessenger debugMessenger;

public:

    VkResult Create(const VulkanLayersAndExtensions& attachments
        , const AppSetting& appSetting);

    static VkResult IncludeDefaultLayersAndExtensions(VulkanLayersAndExtensions&);

    VkInstance Access() const { return instance; }

    VulkanInstance() {}
    ~VulkanInstance();

};
