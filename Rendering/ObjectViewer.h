#ifndef OBJECT_VIEWER_H
#define OBJECT_VIEWER_H

#include <FL/Fl_Gl_Window.H>
#include <FL/gl.h> 
#include <FL/Fl.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_File_Chooser.H>

#include <vector> 
#include <fstream> 
#include "Rendering/Turtle.h" 

#define REFRESH_RATE .001

class ObjectViewer : public Fl_Gl_Window{
protected: 
	int _w,_h; 
	Turtle::TurtleMachine* _turtlem; 

	inline int getWidth() { return _w; } 
	inline int getHeight() { return _h; } 

public: 
	ObjectViewer(Turtle::TurtleMachine* tm, int x, int y, int w, int h, const char* l=0);
	~ObjectViewer(); 
	void draw(); 
	int handle(int flag); 
	void init();

	void resize(int x, int y, int width, int height);

	static void updateCb(void* userdata){
		ObjectViewer* viewer = (ObjectViewer*) userdata; 
		viewer->draw();
		Fl::repeat_timeout(REFRESH_RATE,ObjectViewer::updateCb,userdata);
	}

	static void saveImageBufferCb(Fl_Widget* widget,void* userdata); 

	void removeRepeat(){ 
		Fl::remove_timeout(ObjectViewer::updateCb,NULL); 
	}

	void setOrthographicProjection(); 
};


#endif 