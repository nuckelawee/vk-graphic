#pragma once

#include "app_setting.hpp"

#include "vk_debug_messenger.hpp"

namespace vk {

class Instance {

    VkInstance instance_;
    DebugMessenger debugMessenger_;

public:

    VkResult Create(const LayersAndExtensions& attachments
        , const AppSetting& appSetting);

    static VkResult IncludeDefaultLayersAndExtensions(LayersAndExtensions&);

    VkInstance Access() const { return instance_; }

    Instance() {}
    ~Instance();

};

} //vk
