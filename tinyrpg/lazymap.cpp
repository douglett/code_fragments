#include <vector>
#include <string>
#include <stdlib.h>
#include "lazymap.h"

using namespace std;


const int 
	MAP_WIDTH = 20, 
	MAP_HEIGHT = 20;
static vector<string> map;



namespace lazymap {

	
	vector<string>& getmap() {
		return map;
	}


	int buildmap(int seed) {
		// clear old
		map.erase(map.begin(), map.end());
		// mobs.erase(mobs.begin(), mobs.end());

		// int seed = time(NULL);
		seed = 6000;

		// create map
		srand(seed);
		for (int y = 0; y < 20; y++) {
			string s;
			for (int x = 0; x < 20; x++) {
				if (x == 0 || y == 0 || x == 19 || y == 19)
					s += '#';
				else if (rand()%15 == 0)
					s += '#';
				else {
					// map[y].push_back( rand()%2 );
					rand();
					s += '.';
				}
			}
		}

		// make mobs
		// srand(seed);
		// int mobcount = 15 + rand()%15;
		// for (int i = 0; i < mobcount; i++) {
		// 	mob m;
		// 	m.type = rand()%2 + 1;
		// 	m.name = mob_names[m.type];
		// 	m.x = rand()%17 + 1;
		// 	m.y = rand()%17 + 1;
		// 	mobs.push_back(m);
		// }

		return 0;
	}

}
