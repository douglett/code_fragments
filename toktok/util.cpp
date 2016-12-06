#include <string>
#include "util.h"

using namespace std;


namespace util {

	static char cs[100];
	
	string ccode(col_code col) {
		snprintf(cs, 100, "\033[%dm", col);
		return cs;
	}

	string escapews(const string& s) {
		string s2;
		for (auto c : s)
			switch (c) {
			// case ' ':   s2 += " ";    break; 	// (0x20)	space (SPC)
			case '\t':	s2 += "\\t";  break; 	// (0x09)	horizontal tab (TAB)
			case '\n':  s2 += "\\n";  break;	// (0x0a)	newline (LF)
			case '\v':  s2 += "\\v";  break;	// (0x0b)	vertical tab (VT)
			case '\f':  s2 += "\\f";  break;	// (0x0c)	feed (FF)
			case '\r':  s2 += "\\r";  break;	// (0x0d)	carriage return (CR)
			default:    s2 += c;
			}
		return s2;
	}

	string strtolower(string s) {
		for (auto& c : s)
			c = tolower(c);
		return s;
	}

} // end util