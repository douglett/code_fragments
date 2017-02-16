#pragma once

#include <string>
#include <vector>

namespace pipelib {
	typedef  int (buffercmd_t) (const char* in, const char** out, void* data);
	struct Lib {
		void*         handle;
		buffercmd_t*  buffercmd;
		int           version;
		std::string   name, info;
		std::vector<std::string>  cmdlist;
	};
	extern std::string  dlpath, ext, response;
	extern std::vector<std::vector<char>>  data;
	int  load   (const std::string& name);
	int  unload (const std::string& name);
	Lib* get    (const std::string& name);
	int send    (const std::string& name, const std::string& msg, std::vector<std::vector<char>>* sdata = NULL);
	std::vector<char> vchar (const std::string& str);
} // end pipelib


class DataBlock {
public:
	std::vector<std::vector<char>> data;
	int cpos(int pos);
	uint32_t* to32(int pos);
	void setstr(int pos, const std::string& str);
}; // end DataBlock