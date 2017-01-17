SRC = shader.cpp
OUT = libshader.dylib

$(OUT): $(SRC)
	clang++ -Wall `pkg-config --cflags --libs glfw3 glew` -shared -o $(OUT) $(SRC)

clean:
	rm -f *.o *.out *.dylib
