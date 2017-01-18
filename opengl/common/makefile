SRC = core.cpp shader.cpp objects.cpp objects2.cpp
OUT = libcommon.dylib

$(OUT): $(SRC)
	clang++ -std=c++11 -Wall -framework opengl -shared -Wl,-install_name,`pwd`/$(OUT) \
		`pkg-config --cflags --libs glfw3 glew glm` -o $(OUT) $(SRC)

clean:
	rm -f *.o *.out *.dylib
