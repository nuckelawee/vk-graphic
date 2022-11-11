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
    
    pWindow_ = glfwCreateWindow(appSetting.Width(), appSetting.Height()
        , appSetting.AppName().c_str(), nullptr, nullptr);

    if(pWindow_ == nullptr) {
#ifdef DEBUG
        std::cerr << "ERROR [ GLFW ]\n---> GLFW window creation failed\n\n";
#endif
        exit(EXIT_FAILURE);
    }
}

#ifdef DEBUG
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
#endif

VkResult Window::CreateSurface(const VkInstance& instance) {
    VkResult result = glfwCreateWindowSurface(instance, pWindow_, nullptr, &surface_);
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

SurfaceDetails Window::SurfaceCapabilities(const VkPhysicalDevice& gpu
    , const VkSurfaceKHR& surface) {
    SurfaceDetails details;

    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(gpu, surface
        , &details.capabilities);

    uint32_t formatCount = 0;
    vkGetPhysicalDeviceSurfaceFormatsKHR(gpu, surface, &formatCount, nullptr);
    if(formatCount > 0) { 
        details.formats.resize(formatCount);
        vkGetPhysicalDeviceSurfaceFormatsKHR(gpu, surface, &formatCount
            , details.formats.data());
    }

    uint32_t presentCount;
    vkGetPhysicalDeviceSurfacePresentModesKHR(gpu, surface, &presentCount
        , nullptr);
    if(presentCount > 0) {
        details.presentModes.resize(presentCount);
        vkGetPhysicalDeviceSurfacePresentModesKHR(gpu, surface, &presentCount
            , details.presentModes.data());
    } 
    return details;
}

void Window::DestroySurface(const VkInstance& instance) {
    vkDestroySurfaceKHR(instance, surface_, nullptr);
}

void Window::CloseWindow() {
    glfwSetWindowShouldClose(pWindow_, GLFW_TRUE);
}

bool Window::ShouldClosed() const {
    return glfwWindowShouldClose(pWindow_);
}

Window::~Window() {
    glfwDestroyWindow(pWindow_);
    glfwTerminate();
}
