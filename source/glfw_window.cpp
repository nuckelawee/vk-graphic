#include "glfw_window.hpp"

void Window::CreateWindow(const AppSetting& appSetting) {
    if(glfwInit() == 0) {
#ifdef DEBUG
        std::cerr << "\nERROR [ GLFW ]\n---> GLFW library failed to init\n\n";
#endif 
        exit(EXIT_FAILURE);
    }
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    
    pWindow = glfwCreateWindow(appSetting.Width(), appSetting.Height()
        , appSetting.AppName().c_str(), nullptr, nullptr);

    if(pWindow == nullptr) {
#ifdef DEBUG
        std::cerr << "ERROR [ GLFW ]\n---> GLFW window creation failed\n\n";
#endif
        exit(EXIT_FAILURE);
    }
}

void processSurfaceErrorCreation(VkResult errorType) {
    switch(errorType) {
    case VK_ERROR_INITIALIZATION_FAILED:
        std::cerr << "\nERROR [ GLFW create surface ]\n---> "\
            "The Vulkan loader or at least one minimally functional "\
            "ICD were not found";
        return;
    case VK_ERROR_EXTENSION_NOT_PRESENT:
        std::cerr << "\nERROR [ GLFW create surface ]\n---> "\
            "The required window surface creation instance extensions "\
            "are not available or if the specified instance was not created "\
            "with these extensions enabled\n\n";
        return;
    case VK_ERROR_NATIVE_WINDOW_IN_USE_KHR:
        std::cerr << "\nERROR [ GLFW create surface ]\n---> "\
            "The window surface cannot be shared with another API "\
            "so the window must have been created with the client api hint "\
            "set to GLFW_NO_API otherwise\n\n";
        return;
    default:
        std::cerr << "\nERROR [ GLFW create surface ]\n---> "\
            "Unkown error\n\n";
    }
}

VkResult Window::CreateSurface(const VkInstance& instance) {
    VkResult result = glfwCreateWindowSurface(instance, pWindow, nullptr, &surface);
#ifdef DEBUG
    if(result != VK_SUCCESS) {
        processSurfaceErrorCreation(result); 
        std::cerr << "\nERROR [ GLFW create surface ]\n---> "\
            "Failed to create window surface\n\n";
    }
    assert(result == VK_SUCCESS);
#endif
    return result;
}

void Window::DestroySurface(const VkInstance& instance) {
    vkDestroySurfaceKHR(instance, surface, nullptr);
}

bool Window::ShouldClosed() {
    return glfwWindowShouldClose(pWindow);
}

Window::~Window() {
    glfwDestroyWindow(pWindow);
    glfwTerminate();
}
