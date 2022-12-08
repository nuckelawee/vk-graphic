#include "vk/vk_surface.hpp"

namespace vk {

void Surface::Destroy(const Instance& instance) {
    vkDestroySurfaceKHR(instance.Access(), surface_, nullptr);
}

} // vk
