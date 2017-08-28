#include <iostream>
#include <fstream>
#include <unistd.h>

using namespace std;


int main() {
	fstream fs("/tmp/dswapd/vterm1.swap", fstream::in | fstream::out | fstream::binary);
	if (!fs.is_open())
		return fprintf(stderr, "error: could not open swap file\n"), 1;
	const int SSIZE = 320*240*sizeof(uint32_t);
	char buf[SSIZE] = {0};

	int x=10,y=10;
	while (true) {
		char* p = buf + ((y*320 + x)*sizeof(uint32_t));
		p[0]=p[1]=p[2]=p[3]=0;
		x+=2;
		if (x>=320)  x=0;
		p = buf + ((y*320 + x)*sizeof(uint32_t));
		p[0]=p[1]=p[2]=p[3]=255;

		fs.seekp(128);
		fs.write(buf, SSIZE);

		usleep(100*1000);
	}
}
