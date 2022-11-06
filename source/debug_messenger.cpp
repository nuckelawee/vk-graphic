#include "debug_messenger.hpp"

VKAPI_ATTR VkBool32 VKAPI_CALL DebugMessenger::DebugCallback(
          VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity
        , VkDebugUtilsMessageTypeFlagsEXT messageType
        , const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData
        , void* pUserData) {
    
    if(messageSeverity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT
        && messageType >= VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT) {

        std::cerr << "VALIDATION LAYER:\n" << pCallbackData->pMessage << '\n';
    }
    return VK_FALSE;
}

void DebugMessenger::PopulateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT&
    debugMessengerInfo) {

    debugMessengerInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    debugMessengerInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT
        | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT
        | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
    debugMessengerInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT
        | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
    debugMessengerInfo.pfnUserCallback = DebugCallback;
    debugMessengerInfo.pUserData = nullptr;
}

void DebugMessenger::Setup(const VkInstance& instance) {
    VkDebugUtilsMessengerCreateInfoEXT debugMessengerInfo {};
    PopulateDebugMessengerCreateInfo(debugMessengerInfo);
    if(Create(instance, &debugMessengerInfo, nullptr) != VK_SUCCESS) {
        
        std::cerr << "\nWARNING [ Setup debug messenger ]\n---> "\
            "Failed to create debug messenger\n\n";
    }
}

VkResult DebugMessenger::Create(const VkInstance& instance
    , VkDebugUtilsMessengerCreateInfoEXT *pCreateInfo
    , const VkAllocationCallbacks *pAllocator) {

    auto func = (PFN_vkCreateDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance
        , "vkCreateDebugUtilsMessengerEXT");
    if (func != nullptr) {
        return func(instance, pCreateInfo, pAllocator, &debugMessenger);
    } else {
        return VK_ERROR_EXTENSION_NOT_PRESENT;
    }
}

void DebugMessenger::Destroy(const VkInstance& instance
    , const VkAllocationCallbacks *pAllocator) {

    auto func = (PFN_vkDestroyDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance
        , "vkDestroyDebugUtilsMessengerEXT");
    if (func != nullptr) {
        return func(instance, debugMessenger, pAllocator);
    }
}
