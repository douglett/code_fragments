#pragma once

#include <string>
#include <vector>

namespace scr {
	void init();
	void quit();
	void repaint();
	void repaint_map();
	int  get_input();
	int  end_input();
	int  run_command(std::vector<std::string>& vs);
} // end scr