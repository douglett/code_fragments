#include <iostream>
#include <fstream>

using namespace std;


enum STATE {
	ST_none,
	ST_ws,
	ST_ident
};

void tok(STATE state, const string& s) {
	printf("  %d  [%s]\n", state, s.c_str());
}

int main() {
	fstream fs("main.cpp");
	char c;
	string s;
	STATE state = ST_none;
	while (fs.get(c)) {
		switch (state) {
		case ST_none:
			if (isspace(c))
		case ST_ws:
			if (isspace(c))  s += c;
			else  tok(state, s), s = c;
			break;
		case ST_ident:
		}

		//if (isspace(c)) {
		//	if (s.length() > 0) {
		//		printf("[%s]\n", s.c_str());
		//		s = "";
		//	}
		//} else {
		//	s += c;
		//}
		//cout << c;
	}
}
