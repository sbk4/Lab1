//#include "GUI/MainWindow.h" 
#include "Common/Common.h" 
#include "GUI/FrameWindow.h" 
#include "GUI/CmdInputGroup.h" 
#include "Rendering/Turtle.h" 
#include "Text/Parser.h"
#include "Rendering/ObjectViewer.h" 
#include <FL/Fl.H>
#include <FL/Fl_Button.H>

#include <iostream> 
#include <string> 

using namespace std; 

int main() { 
	Interpreter* intp = new Interpreter(); 

	FrameWindow m(40,40,1024,520,"Lab - Turtle"); 

	ObjectViewer w2(intp->getTurtleMachine(),10,10,500,500,"opengl"); 
	w2.box(FL_FLAT_BOX); 
	w2.color(FL_BLACK); 
	w2.end(); 
	intp->setViewer(&w2);

	CmdInputGroup group(intp,510,15,500,500);
	group.box(FL_FLAT_BOX); 

	int adx=90;

	Fl_Button* saveImageBt = new Fl_Button(230+adx, 520, 100, 20, "Save Image");
	saveImageBt->callback(ObjectViewer::saveImageBufferCb, &w2);
	saveImageBt->color(WIN_COLOR); 
	saveImageBt->box(FL_BORDER_BOX); 

	Fl_Button* loadFileBt = new Fl_Button(380+adx, 520, 100, 20, "Load File");
	loadFileBt->callback(TInput::loadCmdsFromFileCb,group.getInput()); 
	loadFileBt->color(WIN_COLOR); 
	loadFileBt->box(FL_BORDER_BOX); 	

	Fl_Button* saveFileBt = new Fl_Button(520+adx, 520, 100, 20, "Save File");
	saveFileBt->callback(TInput::saveCmdsFromFileCb,group.getInput()); 
	saveFileBt->color(WIN_COLOR); 
	saveFileBt->box(FL_BORDER_BOX); 	

	w2.show(); 
	m.end(); 
	m.show(); 
	return Fl::run(); 
}