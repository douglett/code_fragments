#include "testmap.h"

using namespace std;


namespace testmap {

	vector<string> gmap;
	vector<map<string, int> > gmobs;


	int buildmap(int seed) {
		gmap = vector<string> {
			"########       ##################",
			"#......#       #................#",
			"#.i....#########................#",
			"#...............................#",
			"#......#########................#",
			"#......#       #................#",
			"#......#       #................#",
			"########       #................#",
			"               #................#",
			"               ####.#############",
			"                  #.#            ",
			"                  #.#            ",
			"                  #.#            ",
			"                  #.######       ",
			"                  #......#       ",
			"                  #......#       ",
			"                  #....%.#       ",
			"                  ########       "
		};

		gmobs = vector<map<string, int> > {
			{ {"type", -1}, {"x",3}, {"y",2} },
			{ {"type", 1}, {"x",19}, {"y",2} },
			{ {"type", 1}, {"x",24}, {"y",3} },
			{ {"type", 2}, {"x",30}, {"y",5} },
			{ {"type", 2}, {"x",20}, {"y",8} },
			{ {"type", 1}, {"x",24}, {"y",6} }
		};

		return 0;
	}

}
