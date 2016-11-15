CC=g++ -std=c++11 -Wall
SONAME=install_name
# soname
# lib source
LIBSRC=dlib.cpp
LIBNAME=libdlib.so
# out source
SRC=bus.cpp
OUT=bus.out


all: $(OUT) $(LIBNAME)

$(OUT): $(SRC)
	$(CC) -o $(OUT) $(SRC)
$(LIBNAME): $(LIBSRC)
	$(CC) -shared -Wl,-$(SONAME),$(LIBNAME) -o $(LIBNAME) $(LIBSRC)

run: $(OUT) $(LIBNAME)
	./$(OUT)
clean:
	rm -f *.out *.so