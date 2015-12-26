#include <string>
#include <vector>
#include "globals.h"

using namespace std;


namespace menu {

	void use_card(int card);
	void move_hand(int direction);


	const int HANDPOS_MAX = 3;
	vector<int> cards;
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
		 	if (handpos >= 0 && handpos < cards.size()) {
		 		use_card(handpos);
		 		menu::playeraction(action::ACT_MENU);
		 	}
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


	void use_card(int handpos) {
		int card = cards[handpos];
		cards.erase(cards.begin()+handpos);
		switch (card) {
		 case CARD_SPADE:
		 	break;
		 case CARD_HEART:
		 	action::doheal(&playermob);
		 	break;
		 case CARD_CLUB:
		 	break;
		 case CARD_DIAMOND:
		 	break;
		}
	}

} // end menu
