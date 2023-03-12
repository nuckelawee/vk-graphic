#include "error_manager.hpp"

void ErrorManager::Crash(const std::string& message 
    , const std::string& additional) {

    std::cerr << "\nERROR [ " << additional << " ]\n\t---> " << message << "\n\n";
    exit(EXIT_FAILURE);
}

void ErrorManager::Warning(const std::string& message
    , const std::string& additional) {

    std::cerr << "\nWARNING [ " << additional << " ]\n\t---> " << message << "\n\n";
}

void ErrorManager::Validate(Error error, const std::string& message
    , const std::string& additional) {
    
    if(error.statement_ == false) {
        return;
    }

    switch(error.type_) {
    case ERROR_TYPE_USER:
        break;
    case ERROR_TYPE_UNSOLVABLE:
        Crash(message, additional);
        break;
    case ERROR_TYPE_WARNING:
        Warning(message, additional);
        break;
    case ERROR_TYPE_VULKAN:
        VulkanError(error.vulkanStatus_, message);
        break;
    case ERROR_TYPE_FILE_MANAGER:
        FileError(error.fileStatus_, message);
        break;
    case ERROR_TYPE_UNKNOWN:
        Warning(message, additional);
    default:
        Warning("Error whithout type of ErrorType class", "Error manager");
    } 
}

void ErrorManager::FileError(FileResult status, const std::string& additional) {
    switch(status) {
    case FILE_SUCCESS:
        return;
    case FILE_ERROR_OPEN_FILE:
        Warning("Failed to open file", additional);
        break;
    case FILE_ERROR_TO_READ:
        Warning("Failed to read file", additional);
        break;
    case FILE_ERROR_MEMORY_ALLOCATION:
        Warning("Failed to allocate memory", additional);
        break;
    case FILE_TGA_ERROR_NO_IMAGE:
        Warning("File have not image", additional);
        break;
    case FILE_TGA_ERROR_NOT_SUPPORT:
        Warning("This type of tga image not support", additional);
        break;
    case FILE_TGA_ERROR_FILE_IS_CORRUPTED:
        Warning("File is corrupted", additional);
        break;
    case FILE_TGA_ERROR_NOT_TGA:
        Warning("It is not TGA file format", additional);
        break;
    default:
        return;
    }
}

void ErrorManager::VulkanError(VkResult status, const std::string& additional) {
    switch(status) {
    case VK_SUCCESS:
        return;
    case VK_NOT_READY:
        Warning("A fence or query has not yet completed", additional);
        break;
    case VK_TIMEOUT:
        Warning("A wait operation has not completed in the specified time", additional);
        break;
    case VK_EVENT_SET:
        Warning("An event is signaled", additional);
        break;
    case VK_EVENT_RESET:
        Warning("An event is unsignaled", additional);
        break;
    case VK_INCOMPLETE:
        Warning("A return array was too small for the result", additional);
        break;
    case VK_SUBOPTIMAL_KHR:
        Warning("A swapchain no longer matches the surface properties exactly, "\
            "but can still be used to present to the surface successfully.", additional);
        break;
    case VK_THREAD_IDLE_KHR:
        Warning("A deferred operation is not complete but there is currently "\
            "no work for this thread to do at the time of this call.", additional);
        break;
    case VK_THREAD_DONE_KHR:
        Warning("A deferred operation is not complete but there is no work "\
            "remaining to assign to additional threads.", additional);
        break;
    case VK_OPERATION_DEFERRED_KHR:
        Warning("A deferred operation was requested and at least some of the "\
            "work was deferred.", additional);
        break;
    case VK_OPERATION_NOT_DEFERRED_KHR:
        Warning("A deferred operation was requested and no operations were "\
            "deferred.", additional);
        break;
    case VK_PIPELINE_COMPILE_REQUIRED:
        Warning("A requested pipeline creation would have required compilation, "\
            "but the application requested compilation to not be performed.", additional);
        break;
    case VK_ERROR_OUT_OF_HOST_MEMORY:
        Crash("A host memory allocation has failed.", additional);
    case VK_ERROR_OUT_OF_DEVICE_MEMORY:
        Crash("A device memory allocation has failed.", additional);
    case VK_ERROR_INITIALIZATION_FAILED:
        Crash("Initialization of an object could not be completed for "\
            "implementation-specific reasons.", additional);
    case VK_ERROR_DEVICE_LOST:
        Crash("The logical or physical device has been lost. See Lost Device."
            , additional);
    case VK_ERROR_MEMORY_MAP_FAILED:
        Crash("Mapping of a memory object has failed.", additional);

    case VK_ERROR_LAYER_NOT_PRESENT:
        Crash("A requested layer is not present or could not be loaded."
            , additional);
    case VK_ERROR_EXTENSION_NOT_PRESENT:
        Crash("A requested extension is not supported.", additional);
    case VK_ERROR_FEATURE_NOT_PRESENT:
        Crash("A requested feature is not supported.", additional);
    case VK_ERROR_INCOMPATIBLE_DRIVER:
        Crash("The requested version of Vulkan is not supported by "\
            "the driver or is otherwise incompatible for "\
            "implementation-specific reasons.", additional);
    case VK_ERROR_TOO_MANY_OBJECTS:
        Crash("Too many objects of the type have already been created."
            , additional);
    case VK_ERROR_FORMAT_NOT_SUPPORTED:
        Crash("A requested format is not supported on this device."
            , additional);
    case VK_ERROR_FRAGMENTED_POOL:
        Crash("A pool allocation has failed due to fragmentation of "\
            "the pool’s memory. This must only be returned if no attempt to "\
            "allocate host or device memory was made to accommodate the new "\
            "allocation. This should be returned in preference to "\
            "VK_ERROR_OUT_OF_POOL_MEMORY, but only if the implementation is "\
            "certain that the pool allocation failure was due to fragmentation."
            , additional);
    case VK_ERROR_SURFACE_LOST_KHR:
        Crash("A surface is no longer available.", additional);
    case VK_ERROR_NATIVE_WINDOW_IN_USE_KHR:
        Crash("The requested window is already in use by Vulkan or "\
            "another API in a manner which prevents it from being used again."
            , additional);
    case VK_ERROR_OUT_OF_DATE_KHR:
        Crash("A surface has changed in such a way that it is no longer "\
            "compatible with the swapchain, and further presentation requests "\
            "using the swapchain will fail. Applications must query the new "\
            "surface properties and recreate their swapchain if they wish to "\
            "continue presenting to the surface.", additional);
    case VK_ERROR_INCOMPATIBLE_DISPLAY_KHR:
        Crash("The display used by a swapchain does not use the same "\
            "presentable image layout, or is incompatible in a way that prevents "\
            "sharing an image.", additional);
    case VK_ERROR_INVALID_SHADER_NV:
        Crash("One or more shaders failed to compile or link. More "\
            "details are reported back to the application via VK_EXT_debug_report "\
            "if enabled.", additional);
    case VK_ERROR_OUT_OF_POOL_MEMORY:
        Crash("A pool memory allocation has failed. This must only be "\
            "returned if no attempt to allocate host or device memory was made "\
            "to accommodate the new allocation. If the failure was definitely "\
            "due to fragmentation of the pool, VK_ERROR_FRAGMENTED_POOL should "\
            "be returned instead.", additional);
    case VK_ERROR_INVALID_EXTERNAL_HANDLE:
        Crash("An external handle is not a valid handle of the specified "\
            "type.", additional);
    case VK_ERROR_FRAGMENTATION:
        Crash("A descriptor pool creation has failed due to fragmentation."
            , additional);
    case VK_ERROR_INVALID_DEVICE_ADDRESS_EXT:
        Crash("A buffer creation or memory allocation failed because the "\
            "requested address is not available. A shader group handle "\
            "assignment failed because the requested shader group handle "\
            "information is no longer valid.", additional);
    case VK_ERROR_FULL_SCREEN_EXCLUSIVE_MODE_LOST_EXT:
        Crash("An operation on a swapchain created with "\
            "VK_FULL_SCREEN_EXCLUSIVE_APPLICATION_CONTROLLED_EXT failed as "\
            "it did not have exclusive full-screen access. This may occur "\
            "due to implementation-dependent reasons, outside of the "\
            "application’s control.", additional);
    case VK_ERROR_COMPRESSION_EXHAUSTED_EXT:
        Crash("An image creation failed because internal resources "\
            "required for compression are exhausted. This must only be returned "\
            "when fixed-rate compression is requested.", additional);

#ifdef VK_ENABLE_BETA_EXTENSIONS
    case VK_ERROR_IMAGE_USAGE_NOT_SUPPORTED_KHR:
        Crash("The requested VkImageUsageFlags are not supported."
            , additional);

    case VK_ERROR_VIDEO_PICTURE_LAYOUT_NOT_SUPPORTED_KHR:
        Crash("The requested video picture layout is not supported."
            , additional);
    case VK_ERROR_VIDEO_PROFILE_OPERATION_NOT_SUPPORTED_KHR:
        Crash("A video profile operation specified via "\
            "VkVideoProfileInfoKHR::videoCodecOperation is not supported."
            , additional);
    case VK_ERROR_VIDEO_PROFILE_FORMAT_NOT_SUPPORTED_KHR:
        Crash("Format parameters in a requested VkVideoProfileInfoKHR "\
            "chain are not supported.", additional);

    case VK_ERROR_VIDEO_PROFILE_CODEC_NOT_SUPPORTED_KHR:
        Crash("Codec-specific parameters in a requested "\
            "VkVideoProfileInfoKHR chain are not supported.", additional);
    case VK_ERROR_VIDEO_STD_VERSION_NOT_SUPPORTED_KHR:
        Crash("The specified video Std header version is not supported."
            , additional);
#endif

    default:
        Crash("An unknown error has occurred; either the application "\
            "has provided invalid input, or an implementation failure has occurred."
            , additional);
    }
}
