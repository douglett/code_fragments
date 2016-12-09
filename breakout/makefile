OBJ=bin/*.o

breakout: $(OBJ)
	# mac
	clang++  -F../Frameworks -framework SDL2  -rpath @executable_path/../Frameworks  -o breakout $(OBJ)