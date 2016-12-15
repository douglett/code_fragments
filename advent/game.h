#pragma once

#include <string>
#include <vector>

// enums
enum GAME_MODE {
	MODE_NONE,
	MODE_FIGHT,
	MODE_EXPLORE
};
// structs
struct Mob {
	int maxhp  = 10,
		hp     = 0,
		atk    = 2,
		def    = 0,
		lck    = 50;
	std::string name;
	Mob(std::string name);
};

namespace cstr {
	const int CSTR_MAX = 100;
	extern char cstr[CSTR_MAX];
	std::string strtolower(const std::string& s);
} // end cstr

namespace log {
	const int LOG_LEN = 17, LOG_WIDTH = 29;
	extern int screenw, screenh;
	extern std::string input;
	extern std::vector<std::string> vlog;
	extern std::pair<std::string, uint32_t> title;

	void log(const std::string& s);
	void paint();
	void inputc(char c);
	void delc();
	void clearc();
} // end log

namespace map {
	extern int posx, posy;
	extern std::vector<std::string> umap, map;
	// members
	int         isexit (char dir);
	int         movepos(char dir);
	void        showmap();
	void        nextmap();
	std::string exitstr();
} // end map

// macros
#define strprintf(...)  snprintf(cstr::cstr, cstr::CSTR_MAX, __VA_ARGS__), cstr::cstr
#define lprintf(...)  log::log( (snprintf(cstr::cstr, cstr::CSTR_MAX, __VA_ARGS__), cstr::cstr) )