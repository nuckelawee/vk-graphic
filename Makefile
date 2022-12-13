INCLUDE_PATH = include/

PREFIX = build/
CXX = g++
CXXFLAGS = -Wall -g -std=c++17
DEBUG = -D DEBUG=true
LDFLAGS = -lglfw -lvulkan -ldl -lX11 -lXrandr -lXi -I$(INCLUDE_PATH)
VPATH = source/ : source/vk : shaders/
print-%  : ; @echo $* = $($*)

GLSLC = glslc

SRCMODULES := main.cpp app.cpp vk_extensions.cpp vk_instance.cpp vk_device.cpp glfw_window.cpp vk_debug_messenger.cpp thread_pool.cpp blocking_queue.cpp console.cpp statistic.cpp vk_swapchain.cpp vk_engine.cpp vk_graphic_pipeline.cpp vk_pipeline_states.cpp vk_surface.cpp error_manager.cpp file_manager.cpp vk_command_buffer.cpp vk_command_pool.cpp vk_semaphore.cpp vk_fence.cpp app_setting.cpp

OBJMODULES = $(SRCMODULES:.cpp=.o)

VERTSHADERS := vert_trivial.vert
FRAGSHADERS := frag_trivial.frag
SPVSHADERS = $(VERTSHADERS:.vert=.spv) $(FRAGSHADERS:.frag=.spv)

Debug: debug shaders


#=====================CPP=====================#

debug: $(addprefix $(PREFIX), $(OBJMODULES))
	$(CXX) $(CXXFLAGS) $(LDFLAGS) $^ -o $(PREFIX)$@

$(addprefix $(PREFIX), %.o): %.cpp
	$(CXX) $(CXXFLAGS) $(LDFLAGS) $(DEBUG) $< -c -o $@


#===================SHADERS===================#

shaders: $(addprefix $(PREFIX), $(SPVSHADERS))
	
$(addprefix $(PREFIX), %.spv): %.vert
	$(GLSLC) $< -o $@

$(addprefix $(PREFIX), %.spv): %.frag
	$(GLSLC) $< -o $@


run:
	./build/debug
