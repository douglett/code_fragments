#pragma once

#include <string>
#include <vector>

namespace pipelib {
	typedef  int (buffercmd_t) (const char* in, const char** out, void* data);
	struct Lib {
		void*         handle;
		buffercmd_t*  buffercmd;
		std::string   name, info;
	};
	extern std::string  dlpath, ext, response;
	extern std::vector<std::vector<char>>  data;
	int  load   (const std::string& name);
	int  unload (const std::string& name);
	Lib* get    (const std::string& name);
	int send    (const std::string& name, const std::string& msg);
	std::vector<char> vchar (const std::string& str);
} // end pipelib