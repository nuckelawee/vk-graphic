INCLUDE_PATH = include/
LIBRARIES_PATH = libraries/

PREFIX = build/
CXX = g++
CXXFLAGS = -Wall -g -std=c++17
DEBUG = -D DEBUG=true
LDFLAGS = -lglfw -lvulkan -ldl -lX11 -lXrandr -lXi -I$(INCLUDE_PATH) -I$(LIBRARIES_PATH)
VPATH = source/ : source/vk : source/input :shaders/

print-%  : ; @echo $* = $($*)

GLSLC = glslc

SRCMODULES := main.cpp app.cpp vk_extensions.cpp vk_instance.cpp vk_device.cpp glfw_window.cpp vk_debug_messenger.cpp thread_pool.cpp blocking_queue.cpp console.cpp time.cpp vk_swapchain.cpp vk_engine.cpp vk_graphic_pipeline.cpp vk_pipeline_states.cpp vk_surface.cpp error_manager.cpp file_manager.cpp vk_command_pool.cpp app_setting.cpp vk_vertex_binding.cpp vk_data_loader.cpp vk_command_manager.cpp vk_regulator.cpp camera.cpp vk_descriptor_set.cpp vk_descriptor_pool.cpp input_context.cpp input_controller.cpp input_keyboard.cpp input_mouse.cpp

OBJMODULES = $(SRCMODULES:.cpp=.o)

VERTSHADERS := vert_trivial.vert vert_vertex_buffer.vert vert_ubo.vert vert_texture.vert
FRAGSHADERS := frag_trivial.frag frag_texture.frag
SPVSHADERS = $(VERTSHADERS:.vert=.spv) $(FRAGSHADERS:.frag=.spv)

Debug: debug shaders


#=====================CPP=====================#

delete:
	rm -R build/*

rebuild: delete Debug

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
