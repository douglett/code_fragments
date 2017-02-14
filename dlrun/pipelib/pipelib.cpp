#include <iostream>
#include <string>
#include <vector>
#include <dlfcn.h>
#include "pipelib.h"

using namespace std;


namespace pipelib {
	// private
	static vector<Lib> liblist;
	// externs
	string  dlpath = "./",  ext = ".mac.so",  response;
	vector<vector<char>>  data;

	int load(const string& name) {
		string path = dlpath + name + ext;
		// load
		void* handle = dlopen(path.c_str(), RTLD_LAZY);
		if (handle == NULL) {
			cerr << dlerror() << endl;
			return 1;
		}
		void* cmdfn = dlsym(handle, "buffercmd");
		if (cmdfn == NULL) {
			cerr << dlerror() << endl;
			dlclose(handle);
			return 1;
		}
		// make lib
		Lib lib = { handle, (buffercmd_t*) cmdfn, name };
		// get library info
		const char* resp = NULL;
		lib.buffercmd("info", &resp, (void*) &data);
		if (resp == NULL)  fprintf(stderr, "error getting library info\n");
		else  lib.info = resp;
		// get library command list
		
		// save and report
		liblist.push_back(lib);
		printf("loaded: %s :: %s\n", lib.name.c_str(), lib.info.c_str());
		return 0;
	}

	int unload(const string& name) {
		for (int i = 0; i < liblist.size(); i++)
			if (liblist[i].name == name) {
				liblist.erase(liblist.begin() + i);
				return 1;
			}
		return 0;
	}

	Lib* get(const string& name) {
		for (auto& l : liblist)
			if (l.name == name)
				return &l;
		return NULL;
	}

	int send (const string& name, const string& msg) {
		// load module
		if (get(name) == NULL)
			if (load(name)) {
				fprintf(stderr, "could not load dll: %s\n", name.c_str());
				return 1;
			}
		// send message
		const char* cs = NULL;
		int err = get(name)->buffercmd( msg.c_str(), &cs, (void*) &data );
		response = ( cs != NULL ? cs : "" );  // copy response to string
		// handle error
		if (err)  fprintf(stderr, "error (%d): %s\n", err, response.c_str());
		return  err;
	}

	vector<char> vchar(const string& str) {
		vector<char> vc( str.c_str(), (str.c_str() + str.length() + 1) );
		return vc;
	}
} // end pipelib