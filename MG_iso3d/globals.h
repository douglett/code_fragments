#pragma once

#include "3d.h"
#include <string>
#include <array>
#include <vector>

// models
typedef  std::array<double,6>  line_t;
typedef  std::vector<line_t>   lines_t;
struct Model {
	// globals
	static std::vector<std::pair<char,double>> tglobal;
	// member vars
	std::string id;
	double x=0, y=0, z=0;
	double roll=0, pitch=0, yaw=0;
	double scale=1;
	std::array<int,3> col={{255,255,255}};
	lines_t lines;
	// methods
	void draw() const;
};
Model makemodel(const std::string& type, const std::string& id);