#pragma once

#include <string>

namespace scr {
	void init();
	void quit();
	void repaint();
	void repaint_map();
	void println(const std::string& s);
	int  get_input();
	int  end_input();
} // end scr

namespace interpret {
	int  run_command(const std::string& istr);
} // end interpret

namespace loc {
	int  test_exit(char c);
	int  move(char c);
	std::string exits();
} // end loc
