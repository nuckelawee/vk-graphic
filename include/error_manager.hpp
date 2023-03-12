#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <cassert>
#include <string>
#include <iostream>

#include "file_error.hpp"

enum ErrorType { 
    ERROR_TYPE_USER, 
    ERROR_TYPE_UNSOLVABLE, 
    ERROR_TYPE_WARNING,
    ERROR_TYPE_VULKAN,
    ERROR_TYPE_FILE_MANAGER,
    ERROR_TYPE_UNKNOWN
};

class Error {
    friend class ErrorManager;

    ErrorType type_ = ERROR_TYPE_UNKNOWN;
    VkResult vulkanStatus_ = VK_ERROR_UNKNOWN;
    FileResult fileStatus_ = FILE_TGA_ERROR_NO_IMAGE;
    bool statement_ = true;

public:

    Error() {}
    Error(ErrorType type) : type_(type) {}
    Error(ErrorType type, bool statement) : type_(type), statement_(statement) {}
    Error(ErrorType type, VkResult status) : type_(type), vulkanStatus_(status) {}
    Error(VkResult result) : type_(ERROR_TYPE_VULKAN), vulkanStatus_(result) {}
    Error(FileResult result) : type_(ERROR_TYPE_FILE_MANAGER), fileStatus_(result) {}
};

class ErrorManager {
    
public:
    
    static void Validate(Error error, const std::string& messsage = "Unknown error"
        , const std::string& additional = "Unknown");

private: 

    static void Crash(const std::string& message
        , const std::string& additional);
    static void Warning(const std::string& message = "Unknown error"
        , const std::string& additional = "Unknown");
    static void VulkanError(VkResult status, const std::string& additional = "Unknown");
    static void FileError(FileResult status, const std::string& additional = "Unknown");
    
};
