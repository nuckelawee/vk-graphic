#pragma once

#include "vk_device.hpp"
#include <map>

class Model;

namespace vk {

class ModelStorage;
class GraphicPipeline;

class CommandManager {

public:

    static VkCommandPool CreatePool(VkDevice device, uint32_t queueIndex
        , VkCommandPoolCreateFlags flags);

    static VkCommandBuffer BeginSingleCommand(VkDevice device, VkCommandPool pool);
    static void EndSingleCommand(VkDevice device, VkCommandPool pool
        , VkQueue queue, VkCommandBuffer commandBuffer);

    static std::vector<VkCommandBuffer> CreateCommandBuffers(VkDevice device
        , VkCommandPool pool, uint32_t bufferCount);

    static void RecordDrawCommands(VkCommandBuffer& commandBuffer
        , const std::vector<Model>& models
        , VkFramebuffer framebuffer, VkDescriptorSet descriptorSet
        , GraphicPipeline& pipeline, ModelStorage& ModelStorage);

    static void Submit(VkCommandBuffer *commandBuffers, uint32_t bufferCount
        , VkQueue queue, VkFence fence, VkSubmitInfo& submitInfo);
};

} // vk
