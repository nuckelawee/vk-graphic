#include "vulkan_instance.hpp"
#include "vulkan_device.hpp"
#include "glfw_window.hpp"

int main() {
    AppSetting setting;

    VulkanLayersAndExtensions attachments;

    Window window;
    window.CreateWindow(setting);

    VulkanInstance instance; 
    instance.IncludeDefaultLayersAndExtensions(attachments);
    instance.Create(attachments, setting);

    window.CreateSurface(instance.Access());

    VulkanDevice device;
    device.PickGpu(instance.Access(), window.AccessSurface(), attachments);
    device.CreateLogicalDevice(instance.Access(), window.AccessSurface()
        , attachments);
    device.SetQueueFamilies(window.AccessSurface());

    VulkanLayersAndExtensions::PrintAvailableLayersAndExtensions(device.AccessGpu());

    while(!window.ShouldClosed()) {

    }

    device.DestroyDevice();
    window.DestroySurface(instance.Access());

    return 0;
}
