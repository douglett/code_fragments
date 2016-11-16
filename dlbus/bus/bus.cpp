#include <iostream>
#include <string>
#include <cstring>
#include <map>
#include <vector>
#include <dlfcn.h>
#include "../pipe.h"

using namespace std;


class Plugin {
public:
	// vars
	std::string soname;
	void* handle = NULL;
	PIPE_in_t   PIPE_in  = NULL;
	PIPE_out_t  PIPE_out = NULL;
	PIPE_sig_t  PIPE_sig = NULL;
	char mpipe[PIPE_LEN] = { '\0' };
	// member functions
	Plugin(const std::string& soname);
	void err(const std::string& msg);
	void close();
};

Plugin::Plugin(const string& soname) {
	// open
	this->soname = soname;
	handle = dlopen(soname.c_str(), RTLD_LAZY);
	if (!handle) {
		err(dlerror());
		return;
	}
	// attempt load
	map<string, void*> funclist = {
		{ "PIPE_in",  NULL },
		{ "PIPE_out", NULL },
		{ "PIPE_sig", NULL }
	};
	for (auto& fn : funclist) {
		fn.second = dlsym(handle, fn.first.c_str());
		if (fn.second == NULL) {
			err(dlerror());
			return;
		}
	}
	// save functions
	PIPE_in  = (PIPE_in_t)  funclist["PIPE_in"];
	PIPE_out = (PIPE_out_t) funclist["PIPE_out"];
	PIPE_sig = (PIPE_sig_t) funclist["PIPE_sig"];
}

void Plugin::err(const string& msg) {
	fprintf(stderr, "Plugin error  [%s] :: %s\n", soname.c_str(), msg.c_str());
}

void Plugin::close() {
	if (handle != NULL) {
		dlclose(handle);
		handle = NULL;
	}
}


vector<Plugin> pluglist;


int main() {
	printf("BUS_plugin started...\n");
	Plugin p("../dlib/libdlib.so");
	string s = "test 123";
	p.PIPE_in({ s.size()+1, s.c_str() });
	PIPE_t pp = p.PIPE_out();
	cout << pp.c << endl;

	p.close();
}
