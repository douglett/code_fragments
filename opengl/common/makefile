SRC = shader.cpp core.cpp
OUT = libshader.dylib

$(OUT): $(SRC)
	clang++ -std=c++11 -Wall -framework opengl `pkg-config --cflags --libs glfw3 glew glm` -shared -o $(OUT) $(SRC)

clean:
	rm -f *.o *.out *.dylib
