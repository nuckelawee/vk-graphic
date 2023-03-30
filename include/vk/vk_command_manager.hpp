#pragma once

#include "vk_device.hpp"
#include <map>

namespace vk {

class ModelStorage;
class GraphicPipeline;
class Setting;

enum commandType { COMMAND_TYPE_GRAPHICS, COMMAND_TYPE_TRANSFER };

struct CommandInfo {
    commandType type;
    uint32_t bufferCount;
    uint32_t offset;

    CommandInfo(commandType t = COMMAND_TYPE_GRAPHICS, uint32_t count = 0, uint32_t offset = 0) : type(t), bufferCount(count) {}
};

struct CommandBundle {
    VkCommandPool commandPool;
    std::vector<VkCommandBuffer> commandBuffers;
};
 

class CommandManager {

    std::map<commandType, CommandBundle> commands_;
    VkCommandPool resetTransferQueuePool_;
    VkCommandPool resetGraphicQueuePool_;

public:

    static VkCommandPool CreatePool(VkDevice device, uint32_t queueIndex
        , VkCommandPoolCreateFlags flags);

    static VkCommandBuffer BeginSingleCommand(VkDevice device, VkCommandPool pool);
    static void EndSingleCommand(VkDevice device, VkCommandPool pool
        , VkQueue queue, VkCommandBuffer commandBuffer);

    static std::vector<VkCommandBuffer> CreateCommandBuffers(VkDevice device
        , VkCommandPool pool, uint32_t bufferCount);

    static void RecordDrawCommands(VkCommandBuffer& commandBuffer
        , VkFramebuffer framebuffer, VkDescriptorSet descriptorSet
        , const Setting& setting, GraphicPipeline& pipeline
        , ModelStorage& ModelStorage);

    static void Submit(VkCommandBuffer *commandBuffers, uint32_t bufferCount
        , VkQueue queue, VkFence fence, VkSubmitInfo& submitInfo);
};

} // vk
