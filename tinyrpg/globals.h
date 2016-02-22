#pragma once

#include <string>
#include <vector>
#include <sstream>

// mob types
class mob {
public:
	int x = 0;
	int y = 0;
	int type = 0;
	int hp = 3;
	int maxhp = 3;
	int atk = 1;
	int def = 0;
	std::string name;
};

// visible text
class gtext {
public:
	int x = 0;
	int y = 0;
	int type = 0;
	std::string s;
};

// game state
namespace gamestate {
	enum mode {
		MODE_NONE,
		MODE_GAME,
		MODE_GAMEMENU
	};
	extern int gamemode;
}

// attack actions
namespace action {
	enum Action {
		ACT_NONE,
		ACT_KILL,
		ACT_WEST,
		ACT_EAST,
		ACT_NORTH,
		ACT_SOUTH,
		ACT_ACTION,
		ACT_CANCEL,
		ACT_MENU,
		ACT_SELECT
	};
	int  playeraction(int action);
	void allenemyactions();
	void doactionblock(int x, int y);
	int  dospell(int cardtype);
}

// menu actions 
namespace menu {
	enum cardtypes {
		CARD_SPADE,
		CARD_HEART,
		CARD_CLUB,
		CARD_DIAMOND
	};
	extern std::vector<int> cards;
	extern int handpos;
	int playeraction(int action);
	int reset_cards();
	int givecard();
}

// globals
std::stringstream& ss(int reset = 0);
void reset_level(int reset_player = 0);
gtext create_gtext(int x, int y, std::string s, int type = 0);
void cleardead();
void centercam();
void combatlog(const std::string& s);
extern int dungeon_floor;
extern std::vector<std::string> gmap;
extern std::vector<mob> gmobs;
extern std::vector<mob> effects;
extern std::vector<gtext> gtexts;
extern mob playermob;
