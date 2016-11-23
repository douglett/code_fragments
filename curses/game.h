#pragma once

#include <string>
#include <memory>

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


class Thingum {
private:
public:
	int xpos = 0;
	int ypos = 0;
	std::string name = "";
	std::string description = "";
};

class Wizard: public Thingum {
private:
public:
	Wizard() {	
		name = "Floogleflib... the Wizard";
		description = "A wizard.";
	}
};

namespace loc {
	void setup_thingums();
	std::shared_ptr<Thingum> get_thingums_at();
	void show_thingums();
	int  test_exit(char c);
	int  move(char c);
	std::string submap(int h, int w);
	std::string exits();
} // end loc

