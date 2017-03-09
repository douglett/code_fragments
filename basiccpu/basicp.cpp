#include <iostream>
#include <vector>

using namespace std;


namespace basic {

	void showprog(const vector<uint16_t>& prog) {
		// display
		for (int i = 0; i < prog.size(); i++) {
			const char* s = "";
			if      (i > 0 && i % 8 == 7)  s = "\n";
			else if (i > 0 && i % 4 == 3)  s = "   ";
			printf("%08x %s", prog[i], s);
		}
		printf("\n");
	}

} // end basic