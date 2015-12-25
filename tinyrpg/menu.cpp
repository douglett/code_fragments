#include <string>
#include "globals.h"

using namespace std;


namespace menu {

	int handpos = 0;

	void move_hand(int direction) {
		handpos += direction;
		if (handpos < 0)
			handpos = 0;
		else if (handpos > HANDPOS_MAX)
			handpos = HANDPOS_MAX;
	}

} // end menu