#include <iostream>
#include <vector>

using namespace std;


namespace basic {

	void showprog(const vector<uint16_t>& prog) {
		// display v16
		int ps = prog.size();
		for (int i = 0; i < prog.size(); i++) {
			const char* s = "";
			if      (i < ps-1 && i % 8 == 7)  s = "\n";
			else if (i < ps-1 && i % 4 == 3)  s = "   ";
			printf("%04x %s", prog[i], s);
		}
		printf("\n");
	}

} // end basic