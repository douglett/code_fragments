#ifndef __LAZYMAP_DEF__
	#define __LAZYMAP_DEF__

#include <string>
#include <vector>
#include <map>

namespace lazymap {
	extern std::vector<std::string> gmap;
	extern std::vector<std::map<std::string, int> > gmobs;
	int buildmap(int seed);
}

#endif