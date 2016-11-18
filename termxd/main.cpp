#include <iostream>
#include <xdlib.h>

using namespace std;


static int doloop = 1;
static void keycb(int key, int state) {
	if (state == 0)  return;
	printf("%c\n", (char)key);
	if (key == 'q')  doloop = 0;
}


int main() {
	printf("hello world\n");
	// xd::screen::init( xd::screen::DEFAULT_WIDTH, xd::screen::DEFAULT_HEIGHT, 3 );
	xd::screen::init( 480, 480, 2 );
	xd::screen::keycb = keycb;
	auto* buf = xd::screen::backbuffer;

	while (doloop) {
		for (int i = 0; i < 480/8; i++)
			xd::text::prints(buf->getdata(), "*", i*8, i*8);
			// xd::text::prints(buf->getdata(), "hello world hello world helloworld", 0, i*8);
		if (xd::screen::paint())  break;
	}

	xd::screen::quit();
}
