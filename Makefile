INCLUDE_PATH = include/

CXX = g++
CXXFLAGS = -Wall -g -std=c++17
LDFLAGS = -lglfw -lvulkan -ldl -lX11 -lXrandr -lXi -I$(INCLUDE_PATH)
VPATH = source/ : source/vk : shaders/ : build/ 

SRCMODULES = main.cpp app.cpp vk_extensions.cpp vk_instance.cpp vk_device.cpp glfw_window.cpp vk_debug_messenger.cpp thread_pool.cpp blocking_queue.cpp console.cpp statistic.cpp vk_swapchain.cpp vk_engine.cpp vk_graphic_pipeline.cpp vk_pipeline_states.cpp vk_surface.cpp

OBJMODULES = $(SRCMODULES:.c=.o)

%.o: %.cpp %.hpp
	$(CXX) $(CXXFLAGS) -c $^ -o build/$@

make_debug: $(OBJMODULES)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -D DEBUG=true $^ -o build/$@

make_vert: shaders/trivial.vert
	glslc shaders/trivial.vert -o build/trivial_vert.spv
make_frag: shaders/trivial.frag
	glslc shaders/trivial.frag -o build/trivial_frag.spv

debug: make_debug make_vert make_frag

run_debug:
	./build/make_debug
