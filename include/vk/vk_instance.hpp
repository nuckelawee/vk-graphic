#pragma once

#include "vk_debug_messenger.hpp"

namespace vk {

class LayerAndExtensions;

class Instance {

    VkInstance instance_;
    DebugMessenger debugMessenger_;

public:

    void Create(const LayersAndExtensions& attachments);

    static void IncludeDefaultLayersAndExtensions(LayersAndExtensions&);

    VkInstance Access() const noexcept;

    ~Instance();
};

} //vk
