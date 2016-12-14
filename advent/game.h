#pragma once

#include <string>
#include <vector>

namespace cstr {
	const int CSTR_MAX = 100;
	extern char cstr[CSTR_MAX];
	std::string strtolower(const std::string& s);
} // end cstr

namespace log {
	const int LOG_LEN = 18, LOG_WIDTH = 29;
	extern int dirty, dirtyi;
	extern int screenw, screenh;
	extern std::string input;
	extern std::vector<std::string> vlog;

	void log(const std::string& s);
	void paint();
	void inputc(char c);
	void delc();
} // end log

// macros
#define strprintf(...)  snprintf(cstr::cstr, cstr::CSTR_MAX, __VA_ARGS__), cstr::cstr
#define lprintf(...)  log::log( (snprintf(cstr::cstr, cstr::CSTR_MAX, __VA_ARGS__), cstr::cstr) )