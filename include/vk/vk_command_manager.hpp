#pragma once

#include "vk/vk_graphic_pipeline.hpp"
#include "vk_command_pool.hpp"
#include "vk_regulator.hpp"

#include <map>
#include <vector>

namespace vk {

class DataLoader;

struct CommandInfo {
    commandType type;
    size_t bufferCount;
    size_t offset = 0;

    CommandInfo() {}
    CommandInfo(commandType nType, size_t nBufferCount = 0, size_t nOffset = 0) 
        : type(nType), bufferCount(nBufferCount), offset(nOffset) {}
};

struct CommandBundle {
    CommandPool commandPool;
    std::vector<VkCommandBuffer> commandBuffers;
};
    

class CommandManager {
    std::map<commandType, CommandBundle> commands_;

public:

    CommandInfo Allocate(const Device& device, const CommandInfo& info);

    void RecordDrawCommands(const Device& device
        , const Setting& setting, const GraphicPipeline& pipeline
        , const Swapchain& swapchain, DataLoader& dataLoader
        , const CommandInfo& commandInfo);

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

private:

    CommandBundle& FindBundle(const Device& device, const CommandInfo& info);
    
    CommandManager(const CommandManager&) = delete;
    CommandManager& operator=(const CommandManager) = delete;

};

} // vk
