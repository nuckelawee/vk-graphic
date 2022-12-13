#include "glfw_window.hpp"

void GlfwWindow::CreateWindow() {
    if(glfwInit() == 0) {
#ifdef DEBUG
        std::cerr << "\nERROR [ GLFW ]\n---> GLFW library failed to init\n\n";
#endif 
        exit(EXIT_FAILURE);
    }
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    
    pWindow_ = glfwCreateWindow(AppSetting::Get().Width(), AppSetting::Get().Height()
        , AppSetting::pAppName, nullptr, nullptr);

    if(pWindow_ == nullptr) {
#ifdef DEBUG
        std::cerr << "ERROR [ GLFW ]\n---> GLFW window creation failed\n\n";
#endif
        exit(EXIT_FAILURE);
    }
    glfwSetKeyCallback(pWindow_, KeyCallback);
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

VkResult GlfwWindow::Create(const vk::Instance& instance) {
    VkResult result = glfwCreateWindowSurface(instance.Access(), pWindow_
        , nullptr, &surface_);
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

vk::SurfaceDetails GlfwWindow::Capabilities(const VkPhysicalDevice& gpu) const {
    vk::SurfaceDetails details;

    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(gpu, surface_
        , &details.capabilities);

    uint32_t formatCount = 0;
    vkGetPhysicalDeviceSurfaceFormatsKHR(gpu, surface_, &formatCount, nullptr);
    if(formatCount > 0) { 
        details.formats.resize(formatCount);
        vkGetPhysicalDeviceSurfaceFormatsKHR(gpu, surface_, &formatCount
            , details.formats.data());
    }

    uint32_t presentCount;
    vkGetPhysicalDeviceSurfacePresentModesKHR(gpu, surface_, &presentCount
        , nullptr);
    if(presentCount > 0) {
        details.presentModes.resize(presentCount);
        vkGetPhysicalDeviceSurfacePresentModesKHR(gpu, surface_, &presentCount
            , details.presentModes.data());
    } 

    return details;
}

void GlfwWindow::KeyCallback(GLFWwindow *pWindow, int key, int scancode
    , int action, int modes) {
    
    if(key == GLFW_KEY_ESCAPE) {
        glfwSetWindowShouldClose(pWindow, GLFW_TRUE);
    }
}
void GlfwWindow::CloseWindow() {
    glfwSetWindowShouldClose(pWindow_, GLFW_TRUE);
}

bool GlfwWindow::ShouldClosed() const {
    return glfwWindowShouldClose(pWindow_);
}

GlfwWindow::~GlfwWindow() {
    glfwDestroyWindow(pWindow_);
    glfwTerminate();
}
