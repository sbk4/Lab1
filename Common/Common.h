#ifndef COMMON_H
#define COMMON_H

#include <Fl/Fl.H>
#include <string> 
#include <vector>
#include <sstream> 

using namespace std; 

const Fl_Color WIN_COLOR = fl_rgb_color(244,247,251); 

namespace Str{
	double parseDouble(const string& str); 
	double parseInt(const string& str); 
	string toString(int i); 
	string toString(double  d); 
	bool isLetter(char c); 
	bool isNumber(char c); 
	vector<string> split(const string& s, char c); 
}

#endif