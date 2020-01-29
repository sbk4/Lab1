#ifndef CMD_INPUT_GROUP_H
#define CMD_INPUT_GROUP_H

#include <FL/Fl_Group.H>
#include <FL/Fl_Text_Display.H>
#include <FL/Fl_Text_Editor.H>
#include <FL/Fl.H>

#include "Text/Parser.h" 

#include <iostream> 
#include <string> 
#include <vector>
using namespace std; 

class CmdInputGroup; 

class TDisplay : public Fl_Text_Display{
protected: 
	CmdInputGroup* _group; 
	Fl_Text_Buffer* _buffer; 
	Fl_Text_Buffer* _highlight; 
	int _selected; 

	vector<string> _cmds; 
	vector<pair<pair<int,int>,int> > _cmdRanges; 

protected: 
	void updateCmdDisplay(); 

public: 
	TDisplay(int x, int y, int w, int h, CmdInputGroup* group); 
	int handle(int ev); 
	void addCmd(const string& str); 
	void clearCmds(); 

}; 

class TOutput : public Fl_Text_Display{
protected: 
	Fl_Text_Buffer* _highlight; 
public: 
	TOutput(int x, int y, int w, int h); 
	void good(const string& s); 
	void bad(const string& s); 
}; 


class TInput : public Fl_Text_Editor{
protected: 

	CmdInputGroup* _group; 
	Interpreter* _intp; 
	Fl_Text_Buffer* _highlight; 
	Fl_Text_Buffer* _buffer; 

protected: 
	void inputKey(const string& s, int c); 

public: 
	TInput(Interpreter* intp, int x, int y, int w, int h, CmdInputGroup* group); 
	int handle(int ev); 
	void enterCmds(const vector<string>& cmds); 

	static void loadCmdsFromFileCb(Fl_Widget* widget,void* userdata);
	static void saveCmdsFromFileCb(Fl_Widget* widget,void* userdata);

	friend class CmdInputGroup; 
}; 

class CmdInputGroup : public Fl_Group{
protected: 
	TInput* _input; 
	TDisplay* _display; 
	TOutput* _output; 

protected: 
	void goodCmdEntered(const string& str, const string& res); 
	void badCmdEntered(const string& res); 
	void setInput(const string& str); 
public: 
	CmdInputGroup(Interpreter* intp, int x, int y, int w, int h); 
	TInput* getInput() { return _input; }

	friend class TInput; 
	friend class TDisplay; 
}; 

#endif