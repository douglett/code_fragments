#ifndef __GLOBALS_DEF__
	#define __GLOBALS_DEF__

#include <string>
#include <vector>

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
extern mob playermob;

// game modes
namespace gamemode {
	enum mode {
		MODE_NONE,
		MODE_GAME,
		MODE_GAMEMENU
	};
	extern int mode;
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
	void doheal(mob* target);
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
}

#endif