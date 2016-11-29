#include <iostream>
#include <sstream>
#include <thread>
#include <chrono>

using namespace std;

extern "C" {
	void PIPE_init(void* in, void* out, int* inf, int* outf);
	void PIPE_dothread();
	void PIPE_step();
}

static stringstream  *in = NULL,  *out = NULL;
static int  *infl = NULL,  *outfl = NULL;


void PIPE_init(void* in_, void* out_, int* infl_, int* outfl_) {
	in   = (stringstream*) in_;
	out  = (stringstream*) out_;
	infl  = infl_;
	outfl = outfl_;
}

void PIPE_dothread() {
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
	}
}

void PIPE_step() {
	if (*infl == 0)  return;
	string s;
	cout << "  [";
	while (*in >> s)
		cout << " " << s << " ";
	cout << "]" << endl;
	*infl = 0;
}