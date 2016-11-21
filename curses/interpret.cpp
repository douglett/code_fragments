#include <string>
#include <vector>
#include <sstream>
#include "game.h"

using namespace std;


// helpers
static string tolower(const string& s) {
	string s2 = s;
	for (auto& c : s2)
		c = tolower(c);
	return s2;
}


namespace interpret {

	static vector<string> cmdlist;


	int run_command(const string& istr) {
		// parse string to command list
		cmdlist.erase(cmdlist.begin(), cmdlist.end());
		stringstream ss(tolower(istr));
		string s;
		while (ss >> s)  cmdlist.push_back(s);
		if (cmdlist.size() == 0)  return 0;

		// do command
		string cmd = cmdlist[0];
		if (cmd.substr(0, 2) == ":q" || cmd == "quit")
			return 1;
		else if (cmd.substr(0, 2) == ":h" || cmd == "help")
			scr::println(
				"commands:\n"
				"    :q, quit - exit game\n"
				"    :h, help - this menu\n"
				"    n - north\n"
				"    s - south\n"
				"    e - east\n"
				"    w - west" );
		else if (cmd == "l" || cmd == "look")
			scr::println("exits are: " + position::exits());
		else if (cmd == "n")
			scr::println("you walked north.");
		else if (cmd == "s")
			scr::println("you walked south.");
		else if (cmd == "e")
			scr::println("you walked east.");
		else if (cmd == "w")
			scr::println("you walked west.");
		else
			scr::println("you typed: " + istr);
		return 0;
	}

} // end interpret