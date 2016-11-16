#pragma once

#include <string>
#include "../pipe.h"

namespace Plugin {
	struct Plugin {
		std::string soname;
		void* handle = NULL;
		PIPE_in_t   PIPE_in  = NULL;
		PIPE_out_t  PIPE_out = NULL;
		PIPE_sig_t  PIPE_sig = NULL;
	};
	extern std::string  defpath;
	int     load (const std::string& soname, const std::string& path = "");
	Plugin* find (const std::string& soname);
	int     close(const std::string& soname);
	int     quit ();
} // end Plugin