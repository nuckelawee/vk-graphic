#pragma once

#include "app_setting.hpp"
#include "error_manager.hpp"

#include "vk_debug_messenger.hpp"

namespace vk {

class Instance {

    VkInstance instance_;
    DebugMessenger debugMessenger_;

public:

    void Create(const LayersAndExtensions& attachments
        , const AppSetting& appSetting);

    static void IncludeDefaultLayersAndExtensions(LayersAndExtensions&);

    VkInstance Access() const { return instance_; }

    Instance() {}
    ~Instance();

};

} //vk
