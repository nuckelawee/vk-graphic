INCLUDE_PATH = include/
LIBRARIES_PATH = libraries/

PREFIX = build/
CXX = g++
SANITIZER = #-fsanitize=address
CXXFLAGS = -Wall -g -std=c++17
DEBUG = -D DEBUG=true
LDFLAGS = -lglfw -lvulkan -ldl -lX11 -lXrandr -lXi -I$(INCLUDE_PATH) -I$(LIBRARIES_PATH)
VPATH = source/ : source/vk : source/cmd  source/input : shaders/

print-%  : ; @echo $* = $($*)

GLSLC = glslc

SRCMODULES := main.cpp application.cpp vk_extensions.cpp vk_instance.cpp vk_device.cpp vk_debug_messenger.cpp timer.cpp vk_swapchain.cpp vk_engine.cpp vk_graphic_pipeline.cpp vk_pipeline_states.cpp vk_surface.cpp error_manager.cpp file_manager.cpp global_settings.cpp vk_settings.cpp vk_vertex_binding.cpp vk_command_manager.cpp vk_regulator.cpp base_camera.cpp window.cpp vk_descriptor_set.cpp cmd_command.cpp cmd_instruction.cpp cmd_mouse.cpp cmd_exit.cpp cmd_move_camera.cpp cmd_rotate_camera.cpp input_controller.cpp input_keyboard.cpp input_mouse.cpp vk_buffer_builder.cpp vk_image_builder.cpp vk_model_storage.cpp binary_ifstream.cpp texture.cpp vk_buffer.cpp resource_loader.cpp

OBJMODULES = $(SRCMODULES:.cpp=.o)

VERTSHADERS := vert_trivial.vert vert_vertex_buffer.vert vert_ubo.vert vert_texture.vert
FRAGSHADERS := frag_trivial.frag frag_texture.frag
SPVSHADERS = $(VERTSHADERS:.vert=.spv) $(FRAGSHADERS:.frag=.spv)


#=====================CPP=====================#

debug: $(addprefix $(PREFIX), $(OBJMODULES))
	$(CXX) $(CXXFLAGS) $(DEBUG) $(LDFLAGS) $^ -o $(PREFIX)$@

$(addprefix $(PREFIX), %.o): %.cpp
	$(CXX) $(CXXFLAGS) $(LDFLAGS) $(DEBUG) $< -c -o $@

delete:
	rm -R build/*

rebuild: delete debug shaders

#===================SHADERS===================#

shaders: $(addprefix $(PREFIX), $(SPVSHADERS))
	
$(addprefix $(PREFIX), %.spv): %.vert
	$(GLSLC) $< -o $@

$(addprefix $(PREFIX), %.spv): %.frag
	$(GLSLC) $< -o $@


run:
	./build/debug
