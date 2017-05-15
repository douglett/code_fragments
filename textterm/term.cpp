#include "globals.h"

using namespace std;


namespace term {
	// public vars
	char c_str[C_MAX]={0};
	vector<string> texthist;

	void log(const string& s) {
		texthist.push_back(s);
		printf("%s\n", s.c_str());
	}
} // end term