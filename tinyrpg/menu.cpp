#include <string>
#include "globals.h"

using namespace std;


namespace menu {

	int handpos = 0;
	
	
	int playeraction(int action) {
		switch (action) {
		 case action::ACT_WEST:
		 	move_hand(-1);
			break;
		 case action::ACT_EAST:
		 	move_hand(1);
		 	break;
		 case action::ACT_ACTION:
			break;
		 case action::ACT_MENU:
		 case action::ACT_CANCEL:
		 	gamemode::mode = gamemode::MODE_GAME;
			break;
		 default:
		 	break;
		}
		return 0;
	}
	

	void move_hand(int direction) {
		handpos += direction;
		if (handpos < 0)
			handpos = 0;
		else if (handpos > HANDPOS_MAX)
			handpos = HANDPOS_MAX;
	}

} // end menu
