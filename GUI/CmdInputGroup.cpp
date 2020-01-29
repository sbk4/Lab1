#include "GUI/CmdInputGroup.h" 
#include "Common/Common.H" 
#include <FL/Fl.H>
#include <FL/Fl_File_Chooser.H>

#include <fstream> 

using namespace std; 

#define TS 14 // default editor textsize
Fl_Text_Display::Style_Table_Entry
                   styles[] = {	// Style table
		     { FL_BLACK, FL_COURIER,           TS }, // A - Plain
		     { FL_BLUE, FL_COURIER_BOLD,  TS }, // B - Blue
		     { FL_RED, FL_COURIER_BOLD,  TS } // C - Red
		   };

string repWithNum(const string& s, int num){
	return string(s.length(),num);
}

CmdInputGroup::CmdInputGroup(Interpreter* intp, int x, int y, int w, int h)
: Fl_Group(x,y,w,h){

	string str = ""; 
	int inputw = (w-20)/2; 
	int inputh = h-50; 
	_input = new TInput(intp,x+10,y+10,inputw,inputh,this); 
	_input->box(FL_BORDER_BOX); 
	_input->wrap_mode(1,27); 
	_input->cursor_color(FL_BLUE); 
	_input->cursor_style(Fl_Text_Display::BLOCK_CURSOR); 

	_display = new TDisplay(x+20+inputw,y+10,inputw,inputh+32,this); 
	_display->box(FL_BORDER_BOX);  
	_display->wrap_mode(1,27); 

	_output = new TOutput(x+10,y+h-30,inputw,22); 
	_output->box(FL_BORDER_BOX); 
	end(); 

	this->color(WIN_COLOR); 
}

void CmdInputGroup::goodCmdEntered(const string& str, const string& res){
	_display->addCmd(str); 
	_output->good(res); 
}

void CmdInputGroup::badCmdEntered(const string& res){
	_output->bad(res); 
}

void CmdInputGroup::setInput(const std::string& str){
	_input->buffer()->text(str.c_str()); 
	_input->_highlight->text(repWithNum(str,'A').c_str()); 
	_input->redisplay_range(0,str.length()-1); 
}

TOutput::TOutput(int x, int y, int w, int h) : Fl_Text_Display(x,y,w,h){
	_highlight = new Fl_Text_Buffer(); 
	this->buffer(new Fl_Text_Buffer()); 
	this->highlight_data(_highlight,styles,3,100,NULL,NULL); 
}

void TOutput::good(const string& s){
	this->buffer()->text(s.c_str()); 
	_highlight->text(repWithNum(s,'B').c_str()); 
	this->redisplay_range(0,s.size()-1);
}

void TOutput::bad(const string& s){
	this->buffer()->text(s.c_str()); 
	_highlight->text(repWithNum(s,'C').c_str()); 
	this->redisplay_range(0,s.size()-1);
}

int TDisplay::handle(int ev){
	if(ev==FL_PUSH){
		int x = Fl::event_x()-this->x(); 
		int y = Fl::event_y()-this->y();

		if(x>=0 && x<this->w()-40 && y>=0&&y<this->h()-30){
			x+=this->x(); 
			y+=this->y(); 

			updateCmdDisplay(); 		
			int pos = this->xy_to_position(x,y); 
			_selected = -1; 
			for(int j=0;j<_cmdRanges.size();j++){
				if(pos>=_cmdRanges[j].first.first && pos<=_cmdRanges[j].first.second){
					_selected = _cmdRanges[j].second; 
					break; 
				}
			}

			updateCmdDisplay(); 		

			if(_selected>=0)
				_group->setInput(_cmds[_selected]); 

			return 1; 
		}
		this->buffer()->unselect(); 
	}
	if(ev==FL_RELEASE || ev==FL_DRAG){
		this->buffer()->unselect(); 
	}
	return Fl_Text_Display::handle(ev); 
}

TDisplay::TDisplay(int x, int y, int w, int h, CmdInputGroup* group) 
: Fl_Text_Display(x,y,w,h) 
{ 
	this->show_cursor(0);
	_group = group ;
	_buffer = new Fl_Text_Buffer(); 
	_highlight = new Fl_Text_Buffer(); 
	this->buffer(_buffer); 

	_selected = -1; 

	this->highlight_data(_highlight,styles,3,100,NULL,NULL); 
	this->label(FL_NORMAL_LABEL,"History"); 
} 


void TDisplay::updateCmdDisplay(){
	vector<pair<pair<int,int>,int> > nranges; 

	string bar = "#\n";

	string hlstr = ""; 
	string nstr = ""; 
	nstr+=bar; 
	hlstr+=repWithNum(bar,'A'); 

	// only keep the last 40 commands in history
	int nhist=40; 
	for(int j=_cmds.size()-1,k=0;j>=0&&k<nhist;j--,k++){
		pair<int,int> range = make_pair(nstr.length(),0); 
		nstr+=_cmds[j]+'\n'; 
		if(j==_selected)
			hlstr+=repWithNum(_cmds[j]+'\n','B'); 
		else
			hlstr+=repWithNum(_cmds[j]+'\n','A'); 
		range.second = nstr.length()-1; 
		nstr+= bar; 
		hlstr+=repWithNum(bar,'A'); 
		nranges.push_back(make_pair(range,j)); 
	}
	_cmdRanges.clear(); 
	_cmdRanges.assign(nranges.rbegin(),nranges.rend()); 

	_buffer->text(nstr.c_str()); 
	_highlight->text(hlstr.c_str()); 
	this->redisplay_range(0,nstr.size()-1);
}

void TDisplay::addCmd(const string& str){
	_cmds.push_back(str); 
	updateCmdDisplay(); 
}

void TDisplay::clearCmds(){
	_cmds.clear(); 
	_cmdRanges.clear(); 
	_buffer->remove(0,_buffer->length()-1); 
}

TInput::TInput(Interpreter* intp, int x, int y, int w, int h, CmdInputGroup* group) 
		: Fl_Text_Editor(x,y,w,h) 	{ 
	_intp = intp; 
	_group = group; 
	_buffer = new Fl_Text_Buffer(); 
	_highlight = new Fl_Text_Buffer(); 
	this->label(FL_NORMAL_LABEL,"Input"); 
	this->buffer(_buffer); 
	this->highlight_data(_highlight,styles,3,100,NULL,NULL); 
} 

void TInput::inputKey(const string& s, int c){
	int cursorPos = this->insert_position()-1; 
	int pos=-1; 
	if(c==']'){
		int count=0; 
		for(pos=cursorPos;pos>=0;pos--){
			if(s[pos]==']')
				count++; 
			else if(s[pos]=='[')
				count--; 
			if(count==0)
				break; 
		}
		if(count>0) pos=-1; 
	}

	if(c=='['){
		int count=0; 
		for(pos=cursorPos;pos<s.length();pos++){
			if(s[pos]=='[')
				count++; 
			else if(s[pos]==']')
				count--; 
			if(count==0)
				break; 
		}
		if(count>0) pos=-1; 
	}

	string ss =	repWithNum(string(this->buffer()->text()),'A'); 
	if(pos>=0){
		ss[pos]='C';
		for(int j=min(pos+1,cursorPos+1);j<max(cursorPos,pos);j++)
			ss[j]='B'; 

		ss[cursorPos]='C';
	}

	this->_highlight->text(ss.c_str()); 
	this->redisplay_range(0,ss.size()-1); 
}

int TInput::handle(int ev){
	if(ev==FL_KEYUP){
		if(Fl::event_key()==FL_Enter && Fl::event_shift()){
			string ss(this->buffer()->text()); 
			ss = Parser::trim(ss); 
			if(ss.length()>0){
				pair<bool,string> res = _intp->interpret(ss); 
				if(res.first)
					_group->goodCmdEntered(ss,res.second); 
				else
					_group->badCmdEntered(res.second); 
			}
		}

		inputKey(string(this->buffer()->text()),Fl::event_key()); 
		return 1; 
	}

	// to make sure an enter does not appear for evaluation
	if(ev==FL_KEYDOWN && Fl::event_key()==FL_Enter && Fl::event_shift())
		return 1; 

	return Fl_Text_Editor::handle(ev); 
}

void TInput::enterCmds(const vector<string>& cmds){
	for(unsigned int j=0;j<cmds.size();j++){
		string ss = Parser::trim(cmds[j]); 
		if(ss.length()>0){
			pair<bool,string> res = _intp->interpret(ss); 
			if(res.first)
				_group->goodCmdEntered(ss,res.second); 
			else
				_group->badCmdEntered(res.second); 
		}
	}
}


void TInput::loadCmdsFromFileCb(Fl_Widget* widget,void* userdata){
	TInput* ti = (TInput*) userdata; 

	if(ti){
		char* newfile = fl_file_chooser("Load Commands", ".txt (*.txt)", "./files", 0);
		if(!newfile) return; 

		fstream fs(newfile,fstream::in); 
		string cur=""; 
		vector<string> buf; 
		while(fs.good()){
			char c = fs.get(); 
			if(!fs.good()) 
				break; 

			if(c=='#'){
				if(cur.length()>0)
					buf.push_back(cur); 
				cur = ""; 
			}
			else
				cur+=c; 
		}

		if(cur.length()>0)
			buf.push_back(cur); 

		ti->enterCmds(buf); 
		fs.close(); 
	}
}


void TInput::saveCmdsFromFileCb(Fl_Widget* widget,void* userdata){
	TInput* ti = (TInput*) userdata; 

	if(ti){
		char* newfile = fl_file_chooser("Save Commands", ".txt (*.txt)", "./files", 0);
		if(!newfile) return; 

		fstream fs(newfile,fstream::out); 
		string cur=""; 
		vector<string>* hist = ti->_intp->getHistory(); 
		for(unsigned int j=0;j<hist->size();j++){
			fs<<hist->at(j); 
			fs<<endl<<"#"<<endl;
		}
		fs.close(); 
	}
}