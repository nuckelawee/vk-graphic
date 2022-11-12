INCLUDE_PATH = include/

CXX = g++
CXXFLAGS = -Wall -g -std=c++17
LDFLAGS = -lglfw -lvulkan -ldl -lX11 -lXrandr -lXi -I$(INCLUDE_PATH)
VPATH = source/ : source/vk : shaders/ : build/ 

SRCMODULES = main.cpp vk_extensions.cpp vulkan_instance.cpp vulkan_device.cpp glfw_window.cpp debug_messenger.cpp app.cpp thread_pool.cpp blocking_queue.cpp console.cpp statistic.cpp swapchain.cpp vk_engine.cpp
OBJMODULES = $(SRCMODULES:.c=.o)

%.o: %.cpp %.hpp
	$(CXX) $(CXXFLAGS) -c $^ -o build/$@

debug: $(OBJMODULES)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -D DEBUG=true $^ -o build/$@

run_debug:
	./build/debug
