#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <cassert>
#include <string>
#include <iostream>

enum ErrorType { USER, IMAGE_LOAD, SHADER_LOAD, UNSOLVABLE, WARNING, VULKAN, UNKNOWN };

class Error {
    friend class ErrorManager;

    ErrorType type_ = UNKNOWN;
    VkResult status_ = VK_ERROR_UNKNOWN;
    bool statement_ = true;

public:

    VkResult Status() { return status_; }
    ErrorType Type() { return type_; }
    bool Statement() { return statement_; }

    Error() {}
    Error(ErrorType type) : type_(type) {}
    Error(VkResult status) : type_(VULKAN), status_(status) {}
    Error(ErrorType type, bool statement) : type_(type), statement_(statement) {}
    Error(ErrorType type, VkResult status) : type_(type), status_(status) {}
};

class ErrorManager {
    
public:
    
    static void Validate(Error type, const std::string& messsage = "Unknown error"
        , const std::string& additional = "Unknown");

private: 

    static void Crash(const std::string& message
        , const std::string& additional);
    static void Warning(const std::string& message = "Unknown error"
        , const std::string& additional = "Unknown");
    static void VkError(VkResult status, const std::string& additional = "Unknown");
    
};
