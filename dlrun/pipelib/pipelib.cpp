#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <dlfcn.h>
// #include <cstdio>
#include "pipelib.h"

using namespace std;


namespace pipelib {
	// private
	static vector<Lib> liblist;
	// externs
	string  dlpath = "./",  ext = ".mac.so",  response;
	dblock_t  data;

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
		Lib lib = { handle, (buffercmd_t*) cmdfn, 0, name };
		// get library info
		const char* resp = NULL;
		int result = 0;
		result = lib.buffercmd("info", &resp, (void*) &data);
		if (result || resp == NULL)  fprintf(stderr, "error (%d): no library info (%s)\n", result, name.c_str());
		else  lib.info = resp;
		// get library version number
		result = lib.buffercmd("version", &resp, (void*) &data);
		if (result || resp == NULL)  fprintf(stderr, "error (%d): no library version number (%s)\n", result, name.c_str());
		else  { stringstream ss(resp);  ss >> lib.version; }
		// get library command list
		result = lib.buffercmd("cmdlist", &resp, (void*) &data);
		if (result || resp == NULL)  fprintf(stderr, "error (%d): no command list (%s)\n", result, name.c_str());
		else {
			stringstream ss(resp);  string s;
			while (getline(ss, s))  lib.cmdlist.push_back(s);
		}
		// save and report
		liblist.push_back(lib);
		printf("<loaded> %s (v.%d) :: %s\n", lib.name.c_str(), lib.version, lib.info.c_str());
		return 0;
	}

	int loadall(const vector<string>& names) {
		int err = 0;
		for (const auto& n : names)
			if ((err = load(n)))
				return err;
		return 0;
	}

	int unload(const string& name) {
		for (int i = 0; i < liblist.size(); i++)
			if (liblist[i].name == name) {
				dlclose(liblist[i].handle);
				liblist.erase(liblist.begin() + i);
				return 1;
			}
		return 0;
	}

	int unloadall() {
		while (liblist.size()) {
			dlclose(liblist[0].handle);
			liblist.erase(liblist.begin());
		}
		return 0;
	}

	Lib* get(const string& name) {
		for (auto& l : liblist)
			if (l.name == name)
				return &l;
		return NULL;
	}

	int send(const string& name, const string& msg, dblock_t* sdata) {
		// load module
		Lib* lib = get(name);
		if (lib == NULL) {
			fprintf(stderr, "dll not loaded: %s\n", name.c_str());
			return 1;
		}
		// send message
		const char* cs = NULL;
		if (sdata == NULL)  sdata = &data;
		int err = lib->buffercmd( msg.c_str(), &cs, (void*) sdata );
		response = ( cs != NULL ? cs : "" );  // copy response to string
		// handle error
		if (err)  fprintf(stderr, "error (%d): %s\n", err, response.c_str());
		return  err;
	}

	string exec(const string& cmd) {
		const int BUFSIZE = 128;
		char buffer[BUFSIZE];
		FILE* pipe = popen(cmd.c_str(), "r");
		if (!pipe) {
			fprintf(stderr, "exec: popen() failed\n");
			return "";
		}
		string result;
		while (!feof(pipe))
			if (fgets(buffer, BUFSIZE, pipe) != NULL)
				result += buffer;
		pclose(pipe);
		return result;
	}


// data block helpers

	uint32_t* get32(drow_t& drow) {
		return  (uint32_t*) &drow[0];
	}
	uint32_t* put32(drow_t& drow, const vector<uint32_t>& vec) {
		drow.assign( (char*) &vec[0], (char*) &vec[vec.size()] );
		return  (uint32_t*) &drow[0];
	}
	uint32_t* push32(dblock_t& dblock, const vector<uint32_t>& vec) {
		dblock.emplace_back();
		return  put32(dblock.back(), vec);
	}
	char* putstr(drow_t& drow, const string& str) {
		drow.assign( str.c_str(), (str.c_str() + str.length() + 1) );
		return  &drow[0];
	}
	char* pushstr(dblock_t& dblock, const string& str) {
		dblock.emplace_back();
		return  putstr(dblock.back(), str);
	}
} // end pipelib