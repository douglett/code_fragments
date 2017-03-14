// helper functions
// 
#pragma once

// #include <iostream>
#include <string>
#include <vector>
#include <sstream>

template<typename ... Args> 
std::string 
strfmt(const std::string& fmt, Args ... args) {
	int size = snprintf(NULL, 0, fmt.c_str(), args ...) + 1; // Extra space for '\0'
	char cs[size];
	snprintf(cs, size, fmt.c_str(), args ...);
	return cs;
}
inline std::vector<std::string> 
strsplit(const std::string& str) {
	std::stringstream ss(str);
	std::vector<std::string> vs;
	std::string s;
	while (ss >> s)  vs.push_back(s);
	return vs;
}
inline std::string
strjoin(const std::vector<std::string>& vs) {
	std::string s;
	for (int i=0; i<vs.size(); i++)
		s += vs[i] + (i<vs.size()-1 ? " " : "");
	return s;
}