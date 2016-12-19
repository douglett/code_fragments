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
	int maxhp  = 2,
		hp     = 0,
		atk    = 1,
		def    = 0,
		lck    = 5;
	std::string name;
	Mob(std::string name);
};

namespace util {
	const int CSTR_MAX = 100;
	extern char cstr[CSTR_MAX];
	std::string strtolower(const std::string& s);
	int indexOf(const std::string& needle, const std::vector<std::string>& haystack);
} // end cstr

namespace log {
	const int LOG_LEN = 17, LOG_WIDTH = 29;
	extern int screenw, screenh;
	extern std::string input;
	extern std::vector<std::string> vlog;
	extern std::pair<std::string, uint32_t> title;

	void log(const std::string& s);
	void cls();
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
	char        current();
	std::string roomdesc();
	std::string exitstr();
} // end map

// macros
#define strprintf(...)  snprintf(util::cstr, util::CSTR_MAX, __VA_ARGS__), util::cstr
#define lprintf(...)  log::log( (snprintf(util::cstr, util::CSTR_MAX, __VA_ARGS__), util::cstr) )