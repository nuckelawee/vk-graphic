#pragma once

#include "vk/vk_graphic_pipeline.hpp"
#include "vk_regulator.hpp"
#include "vk_data_loader.hpp"

#include <map>
#include <vector>

namespace vk {

struct CommandBundle {
    CommandPool commandPool;
    std::vector<VkCommandBuffer> commandBuffers;
};
    

class CommandManager {
    std::map<commandType, CommandBundle> commands_;
    CommandPool resetTransferQueuePool_;
    CommandPool resetGraphicQueuePool_;

public:

    void Create(const Device& device);

    CommandInfo Allocate(const Device& device, const CommandInfo& info);

    void RecordDrawCommands(const Device& device
        , const Setting& setting, const GraphicPipeline& pipeline
        , const Swapchain& swapchain, DataLoader& dataLoader
        , const CommandInfo& commandInfo, const DescriptorSet& descriptorSet);

    void Submit(const Device& device, const Setting& setting
        , Regulator& regulator, VkSubmitInfo& submitInfo
        , const CommandInfo& commanInfo);

    const CommandBundle& Access(commandType type) const;
    const VkCommandBuffer* Access(const CommandInfo& info) const;

    void FreeCommandBuffers(const Device& device, commandType type);
    void FreeCommandBuffers(const Device& device, const CommandInfo& info);
    void DestroyCommandPool(const Device& device, commandType type);
    void Destroy(const Device& device);

    CommandManager() {}
    ~CommandManager() {}

    VkCommandBuffer BeginSingleCommand(const Device& device, commandType type);
    void EndSingleCommand(const Device& device, VkCommandBuffer commandBuffer
        , commandType type);

    void CopyBuffer(const Device& device, VkBuffer source, VkBuffer destination
        , VkDeviceSize size);
    void CopyBufferToImage(const Device& device, VkBuffer buffer, VkImage image
        , uint32_t width, uint32_t height);

private:

    CommandBundle& FindBundle(const Device& device, const CommandInfo& info);
    
    CommandManager(const CommandManager&) = delete;
    CommandManager& operator=(const CommandManager) = delete;

};

} // vk
