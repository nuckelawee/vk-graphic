#include "glfw_window.hpp"

void GlfwWindow::CreateWindow() {
    ErrorManager::Validate(Error(UNSOLVABLE, glfwInit() == 0)
        , "GLFW library failed to init", "GLFW window creation");

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
    
    pWindow_ = glfwCreateWindow(AppSetting::Get().Width(), AppSetting::Get().Height()
        , AppSetting::pAppName, nullptr, nullptr);

    ErrorManager::Validate(Error(UNSOLVABLE, pWindow_ == nullptr)
        , "GLFW window creation failed", "GLFW window creation");

    glfwSetKeyCallback(pWindow_, KeyCallback);
    glfwSetFramebufferSizeCallback(pWindow_, FramebufferResize);
}

VkResult GlfwWindow::Create(const vk::Instance& instance) {
    VkResult result = glfwCreateWindowSurface(instance.Access(), pWindow_
        , nullptr, &surface_);
    ErrorManager::Validate(result, "GLFW surface creation");
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

void GlfwWindow::FramebufferResize(GLFWwindow *pWindow, int width, int height) {
    
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
