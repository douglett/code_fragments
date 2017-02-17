#pragma once

#include <string>
#include <vector>

namespace pipelib {
	typedef  int (buffercmd_t) (const char* in, const char** out, void* data);
	typedef  std::vector<std::vector<char>>  dblock_t;
	typedef  std::vector<char>  drow_t;
	struct Lib {
		void*         handle;
		buffercmd_t*  buffercmd;
		int           version;
		std::string   name, info;
		std::vector<std::string>  cmdlist;
	};
	extern std::string  dlpath, ext, response;
	extern dblock_t  data;
	int  load      (const std::string& name);
	int  loadall   (const std::vector<std::string>& names);
	int  unload    (const std::string& name);
	int  unloadall ();
	Lib* get       (const std::string& name);
	int  send      (const std::string& name, const std::string& msg, dblock_t* sdata = NULL);
	// data block helpers
	uint32_t*  get32   (drow_t&   drow);
	uint32_t*  put32   (drow_t&   drow,    const std::vector<uint32_t>& vec);
	uint32_t*  push32  (dblock_t& dblock,  const std::vector<uint32_t>& vec);
	char*      putstr  (drow_t&   drow,    const std::string& str);
	char*      pushstr (dblock_t& dblock,  const std::string& str);
} // end pipelib