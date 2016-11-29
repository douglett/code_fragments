#include <iostream>
#include <sstream>
#include <thread>
#include <chrono>

extern "C" {
	void initpipe(void* in, void* out, int* inf, int* outf);
	void dothread();
	void step();
}

using namespace std;

static stringstream  *in = NULL,  *out = NULL;
static int  *infl = NULL,  *outfl = NULL;


void initpipe(void* in_, void* out_, int* infl_, int* outfl_) {
	in   = (stringstream*) in_;
	out  = (stringstream*) out_;
	infl  = infl_;
	outfl = outfl_;
}

void dothread() {
	string s;
	while (true) {
		if (*infl < 0)  break;
		if (*infl > 0) {
			cout << "  [";
			while (*in >> s)
				cout << " " << s << " ";
			cout << "]" << endl;
			*infl = 0;
		}
		this_thread::sleep_for( chrono::milliseconds(1000) );
		// this_thread::yield();
	}
}

void step() {
	if (*infl == 0)  return;
	string s;
	cout << "  [";
	while (*in >> s)
		cout << " " << s << " ";
	cout << "]" << endl;
	*infl = 0;
}