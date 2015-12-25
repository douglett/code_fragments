#ifndef __GLOBALS_DEF__
	#define __GLOBALS_DEF__

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
}

// menu actions 
namespace menu {
	const int HANDPOS_MAX = 3;
	extern int handpos;
	int playeraction(int action);
	void move_hand(int direction);
}

#endif