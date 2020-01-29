#include "Rendering/ObjectViewer.h"
#include <FL/gl.h> 
#include <GL/glu.h>

#include <iostream>
using namespace std; 

extern "C"{
#include "Common/bmpfile.h"
}

ObjectViewer::ObjectViewer(Turtle::TurtleMachine* tm, int x, int y, int w, int h, const char* l)
: Fl_Gl_Window(x,y,w,h,l){
	//Fl::repeat_timeout(REFRESH_RATE,ObjectViewer::updateCb,this);
	_w = w; 
	_h = h; 
	_turtlem= tm; 
	this->border(5); 
}

ObjectViewer::~ObjectViewer(){
	//Fl::remove_timeout(ObjectViewer::updateCb,this); 
}

void ObjectViewer::setOrthographicProjection(){
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	gluOrtho2D(0, getWidth(), 0, getHeight());
	glTranslatef(getWidth()/2,getHeight()/2,0.f); 
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();
}

void ObjectViewer::draw(){
	if(!valid())
		init();
	setOrthographicProjection(); 
	glClear(GL_COLOR_BUFFER_BIT); 
	if(_turtlem){
		vector<Turtle::Line>* lines = _turtlem->getLines(); 

		for(unsigned int j=0;j<lines->size();j++){
			Turtle::Line& l = lines->at(j); 
			glLineWidth(l.width); 
			glBegin(GL_LINES); 
			glColor3d(l.c.r,l.c.g,l.c.b); 
			glVertex2d(l.a.x,l.a.y); 
			glVertex2d(l.b.x,l.b.y); 
			glEnd(); 
		}
	}
	swap_buffers(); 
}

int ObjectViewer::handle(int ev){
	return Fl_Gl_Window::handle(ev);
}

void ObjectViewer::init(){
	glClearColor(0,0,0,1); 
	glMatrixMode(GL_PROJECTION); 
	glOrtho(0,this->w(),0,this->h(),-100,100); 
	glMatrixMode(GL_MODELVIEW); 
	glLoadIdentity(); 
	glDisable(GL_DEPTH_TEST); 
}

void ObjectViewer::resize(int x, int y, int width, int height){
	make_current();
	_w = width; 
	_h = height; 
	Fl_Gl_Window::resize(x,y,width,height);
}

void ObjectViewer::saveImageBufferCb(Fl_Widget* widget, void* userdata){
	ObjectViewer* viewer = (ObjectViewer*) userdata; 

	if(viewer){
		char* newfile = fl_file_chooser("Save image", ".bmp (*.bmp)", "./images", 0);
		if(!newfile) return; 

		int h = viewer->getHeight(); 
		int w = viewer->getWidth(); 

		GLubyte *data = new GLubyte[4*h*w]; 
		glReadPixels(0,0,w,h, GL_RGBA, GL_UNSIGNED_BYTE, data);

		bmpfile_t* bfile = bmp_create(w,h,32); 
		for(int j=0;j<h;j++){
			for(int i=0;i<w;i++){
				rgb_pixel_t pix = {data[(j*w+i)*4+2],data[(j*w+i)*4+1],data[(j*w+i)*4],data[(j*w+i)*4+3]}; 
				bmp_set_pixel(bfile,i,(h-1)-j,pix); 
			}
		}
		bmp_save(bfile,newfile); 
		bmp_destroy(bfile); 

		delete [] data; 
//		delete [] newfile; 
	}
}