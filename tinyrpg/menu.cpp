#include <string>
#include <vector>
#include "globals.h"

using namespace std;


namespace menu {

	int  use_card();
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
		 	if ( use_card() ) {
		 		menu::playeraction(action::ACT_MENU);  // close menu
		 		return 1;
		 	}
			break;
		 case action::ACT_MENU:
		 case action::ACT_CANCEL:
		 	gamestate::gamemode = gamestate::MODE_GAME;
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


	int use_card() {
		if (handpos < 0 || handpos >= cards.size())
			return 0;
		int card = cards[handpos];
		int action_performed = 0;
		// try to use card
		switch (card) {
		 case CARD_SPADE:
		 case CARD_HEART:
		 case CARD_CLUB:
		 case CARD_DIAMOND:
		 	action_performed = action::dospell(card);
		 	break;
		}
		// check if card was used correctly
		if (action_performed) {
			cards.erase(cards.begin()+handpos);
			return 1;
		} else {
			return 0;
		}
	}


	int givecard() {
		if (cards.size() >= 4)
			return 0;
		cards.push_back( rand() % 4 );
		return 1;
	}

} // end menu
