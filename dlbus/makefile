CC=g++ -std=c++11 -Wall
SONAME=soname
ifeq ($(UNAME_S),Darwin)
	SONAME=install_name
endif
# lib source
LIBSRC=dlib.cpp
LIBNAME=libdlib.so
# out source
SRC=bus.cpp
OUT=bus.out


all: $(OUT) $(LIBNAME)

$(OUT): $(SRC)
	$(CC) -Wl,-rpath,./ -o $(OUT) $(SRC) -ldl
$(LIBNAME): $(LIBSRC)
	$(CC) -shared -Wl,-$(SONAME),$(LIBNAME) -o $(LIBNAME) $(LIBSRC)

run: $(OUT) $(LIBNAME)
	./$(OUT)
clean:
	rm -f *.out *.so
