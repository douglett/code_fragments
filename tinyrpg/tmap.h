#ifndef __TMAP_DEF__
	#define __TMAP_DEF__

#include <string>
#include <vector>
#include <map>

namespace tmap {
	extern std::vector<std::string> gmap;
	extern std::vector<std::map<std::string, int> > gmobs;
	int buildmap(int seed);
}

#endif