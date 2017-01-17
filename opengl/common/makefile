SRC = shader.cpp core.cpp
OUT = libcommon.dylib
INSTALL_NAME =-install_name,`pwd`/$(OUT)

$(OUT): $(SRC)
	clang++ -std=c++11 -Wall -framework opengl -shared -Wl,$(INSTALL_NAME) `pkg-config --cflags --libs glfw3 glew glm` -o $(OUT) $(SRC)

clean:
	rm -f *.o *.out *.dylib
