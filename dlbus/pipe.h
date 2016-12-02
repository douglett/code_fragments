#pragma once

#include <string>
#include <memory>

typedef  void (*voidfn_t) ();
typedef  void (*pipefn_t) (void*, void*, int*, int*);

class pipe {
public:
	void* handle         = NULL;
	pipefn_t pipeinit    = NULL;
	voidfn_t pipethread  = NULL;
	voidfn_t pipestep    = NULL;
	std::string name;
	std::stringstream  in, out;
	int  infl = 0,  outfl = 0;
};

int load   ();
int load   (std::shared_ptr<pipe> p, std::string path, std::string name);
int unload ();
int unload (std::shared_ptr<pipe> p);
std::string message(std::shared_ptr<pipe> p, const std::string& msg);