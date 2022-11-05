#pragma once

class VulkanInstance {

    VkInstance instance;

public:

    VkResult Create(VulkanLayersAndExtensions& attachments
        , const char* pAppName);

    VkInstance& Access() { return instance; }

    VulkanInstance();
    ~VulkanInstance();

};
