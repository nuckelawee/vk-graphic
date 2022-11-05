INCLUDE_PATH = include/

CXX = g++
CXXFLAGS = -Wall -g -std=c++17
LDFLAGS = -lglfw -lvulkan -ldl -lX11 -lXrandr -lXi -I$(INCLUDE_PATH)
VPATH = source/ : shaders/ : build/

SRCMODULES = main.cpp vk_extensions.cpp 
OBJMODULES = $(SRCMODULES:.c=.o)

%.o: %.cpp %.hpp
	$(CXX) $(CXXFLAGS) -c $^ -o build/$@

debug: $(OBJMODULES)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -D DEBUG=true $^ -o build/$@

run_debug:
	./build/debug
