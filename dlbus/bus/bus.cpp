#include <iostream>
#include <string>
#include <cstring>
#include <cassert>
#include <map>
#include <vector>
#include <dlfcn.h>

#include "bus.h"

using namespace std;

typedef  unsigned int  uint;


namespace Plugin {

	static vector<Plugin*> pluglist;
	string defpath = "./";


	// *** private functions ***
	
	static void err(const string& soname, const string& msg) {
		fprintf(stderr, "BUS_plugin: error [%s] :: %s\n", soname.c_str(), msg.c_str());
	}

	static void out(const string& soname, const string& msg) {
		printf("BUS_plugin: [%s] :: %s\n", soname.c_str(), msg.c_str());
	}

	static void close(Plugin* plug) {
		assert(plug != NULL);
		if (plug->handle != NULL) {
			dlclose(plug->handle);
			plug->handle = NULL;
			out(plug->soname, "closed library");
		}
	}


	// *** public functions ***

	int load(const string& soname, const string& path) {
		assert(find(soname) == NULL);
		// open
		void* handle = dlopen((defpath + path + soname).c_str(), RTLD_LAZY);
		if (handle == NULL) {
			err(soname, dlerror());
			return 1;
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
				err(soname, dlerror());
				dlclose(handle);
				return 1;
			}
		}
		// save functions
		Plugin* plug   = new Plugin();
		plug->soname   = soname;
		plug->handle   = handle;
		plug->PIPE_in  = (PIPE_in_t)  funclist["PIPE_in"];
		plug->PIPE_out = (PIPE_out_t) funclist["PIPE_out"];
		plug->PIPE_sig = (PIPE_sig_t) funclist["PIPE_sig"];
		pluglist.push_back(plug);
		out(soname, "library loaded");
		return 0;
	}

	Plugin* find(const string& soname) {
		for (auto* p : pluglist)
			if (p->soname == soname)
				return p;
		return NULL;
	}

	int close(const string& soname) {
		for (auto p = pluglist.begin(); p != pluglist.end(); p++)
			if ((*p)->soname == soname) {
				close(*p);
				pluglist.erase(p);
				return 0;
			}
		err(soname, "closing... could not find library");
		return 1;
	}

	int quit() {
		while (pluglist.size() > 0) {
			close(pluglist[0]);
			pluglist.erase(pluglist.begin());
		}
		return 0;
	}

} // end Plugin



int main() {
	printf("BUS_plugin started...\n");

	// load plugin
	Plugin::defpath = "../dlib/";
	Plugin::load("libdlib.so");
	auto* dlib = Plugin::find("libdlib.so");

	// create pipe
	const uint BUFS = 100;
	char s[BUFS] = "test 123";
	PIPE_t mpipe = { BUFS, s };

	// send a message and get result
	dlib->PIPE_in(mpipe);
	while (dlib->PIPE_sig() > 0) {
		PIPE_t pp = dlib->PIPE_out();
		cout << pp.c << endl;
	}

	Plugin::quit();
}
