#include <iostream>
#include <sstream>
#include <thread>
#include <map>
#include <dlfcn.h>
#include "pipe.h"

using namespace std;


static void* handle         = NULL;
static pipefn_t pipeinit    = NULL;
static voidfn_t pipethread  = NULL;
static voidfn_t pipestep    = NULL;


int load(string path) {
	if (handle != NULL) {
		cerr << "pipe handle not empty" << endl;
		return 1;
	}
	handle = dlopen(path.c_str(), RTLD_LAZY);
	if (handle == NULL) {
		// err(soname, dlerror());
		cerr << dlerror() << endl;
		return 1;
	}
	// attempt load
	map<string, void*> funclist = {
		{ "pipeinit",    NULL },
		{ "pipethread",  NULL },
		{ "pipestep",    NULL }
	};
	for (auto& fn : funclist) {
		fn.second = dlsym(handle, fn.first.c_str());
		if (fn.second == NULL) {
			// err(soname, dlerror());
			cerr << dlerror() << endl;
			dlclose(handle);
			return 1;
		}
	}
	// save
	pipeinit    = (pipefn_t) funclist["pipeinit"];
	pipethread  = (voidfn_t) funclist["pipethread"];
	pipestep    = (voidfn_t) funclist["pipestep"];
	return 0;
}

int load(shared_ptr<pipe> p, string path, string name) {
	int err = load(path + name);
	if (err)  return err;
	// save handles
	p->handle      = handle;
	p->pipeinit    = pipeinit;
	p->pipethread  = pipethread;
	p->pipestep    = pipestep;
	p->name        = name;
	// nullify main handle
	handle      = NULL;
	pipeinit    = NULL; 
	pipethread  = NULL;
	pipestep    = NULL;
	// init pipe
	p->pipeinit(&p->in, &p->out, &p->infl, &p->outfl);
	return 0;
}

int unload() {
	dlclose(handle);
	handle      = NULL;
	pipeinit    = NULL; 
	pipethread  = NULL;
	pipestep    = NULL;
	return 0;
}

int unload(shared_ptr<pipe> p) {
	dlclose(p->handle);
	p->handle      = NULL;
	p->pipeinit    = NULL; 
	p->pipethread  = NULL;
	p->pipestep    = NULL;
	p->name        = "";
	return 0;
}

string message(shared_ptr<pipe> p, const string& msg) {
	// clear
	p->in.str(""), p->in.clear();
	// send
	p->in << msg;
	p->infl = 1;
	p->pipestep();
	// response
	string resp;
	if (p->outfl > 0) {
		getline(p->out, resp);
		p->out.str(""), p->out.clear();
		p->outfl = 0;
	}
	return resp;
}