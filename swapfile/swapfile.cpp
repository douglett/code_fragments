#include <iostream>
#include <string>
#include <fstream>

using namespace std;


class Swap {
public:
	static const int S_HEAD = 32, S_MSG = 512, S_BODY = 512;
	string fname = "swap.txt";
	fstream fs;
	// header
	char head[S_HEAD] = { 0 };
	uint32_t *mwcount = NULL, *dwcount = NULL;

	int create() {
		fs.close();  // just in case
		fs.open(fname, fstream::binary | fstream::in | fstream::out);  // will fail if no file
		if (!fs.is_open()) {
			fs.open(fname, fstream::binary | fstream::out);  // create & overwrite
			if (!fs.is_open()) {
				fprintf(stderr, "could not create swap file: %s\n", fname.c_str());
				return 1;
			}
			printf("creating swap file: %s\n", fname.c_str());
			fs.seekp(S_HEAD + S_MSG + S_BODY - 1);  // assign file size
			fs.write("", 1);
			fs.close();
			fs.open(fname, fstream::binary | fstream::in | fstream::out);  // should succeed at this point
		}
		update();
		mwcount = (uint32_t*)(head + 16);
		dwcount = (uint32_t*)(head + 20);
		return 0;
	}

	// update swap file header info
	int update() {
		if (!fs.is_open())  return 1;
		fs.seekp(0);
		fs.get(head, S_HEAD);
		return 0;
	}

	int writehead() {
		if (!fs.is_open())  return 1;
		fs.seekp(0);
		fs.write(head, S_HEAD);
		return 0;
	}

	// write to swap message (daemon instruction message)
	int writemsg(string msg) {
		if (!fs.is_open())  return 1;
		fs.seekp(S_HEAD + S_MSG);
		fs.write(msg.c_str(), min( (int)msg.length(), (int)S_BODY ));
		*mwcount += 1;  // increment instruction write count
		writehead();
		return 0;
	}

	// write to swap body (daemon results)
	int writedata(string msg) {
		if (!fs.is_open())  return 1;
		fs.seekp(S_HEAD + S_MSG);
		fs.write(msg.c_str(), min( (int)msg.length(), (int)S_BODY ));
		*dwcount += 1;  // increment data write count
		writehead();
		return 0;
	}

	string readmsg() {
		char s[S_MSG] = { 0 };
		fs.seekp(S_HEAD);
		fs.read(s, S_MSG);
		return s;
	}

	string readdata() {
		char s[S_BODY] = { 0 };
		fs.seekp(S_HEAD + S_MSG);
		fs.read(s, S_BODY);
		return s;
	}
}; // end Swap


int main() {
	Swap swp;
	if (swp.create())  return 1;
	
	// head test 1
	// char head[32] = {0};
	// head[0] = head[1] = 1;
	// head[2] = head[3] = 2;
	// printf("%08x \n", *(uint32_t*)head);
	// *(uint32_t*)head = 0x02020101;
	// printf("%08x \n", *(uint32_t*)head);
	// printf("%02x %02x %02x %02x \n", head[0], head[1], head[2], head[3]);

	swp.writedata("hello world");
	// printf("%s\n", swp.readdata().c_str());
}