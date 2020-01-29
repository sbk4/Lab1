#ifndef TURTLE_H
#define TURTLE_H

#include <iostream> 
#include <vector> 
#include <cmath> 

#define M_PI 3.14159265359

using namespace std; 

namespace Turtle{

class Pt{
public: 
	Pt() { x=0;y=0;}
	double x,y; 
}; 

class Color{
public: 
	Color() { r=1;g=1;b=1;}
	double r,g,b; 
};

class Line{
public: 
	Line() { width=1;}
	Pt a,b; 
	Color c; 
	double width; 
}; 

class TurtleState{
public: 
	TurtleState() {	dir = 0; }
	Pt p; 
	double dir; // angle in radians
}; 

// keeps the turtle and a history of its trails 
class TurtleMachine{
protected: 
	TurtleState _turtle; 
	vector<Line> _trails; 
	Color _color; 
	double _width; 
	double _scale; 
public: 
	TurtleMachine(){ reset(); }

	vector<Line>* getLines() { return &_trails; }

	void reset() { 
		// empty trails
		_trails = vector<Line>();

		// Set color to white
		_color.r = 255;
		_color.g = 255;
		_color.b = 255;

		// turtle back at the origin
		_turtle.p.x = 0;
		_turtle.p.y = 0;

		// facing right
		_turtle.dir = 0;

		// line size and scale are 1
		_width = 1;
		_scale = 1;
	}

	void forward(double step) { 
		Line newLine = Line();
		newLine.a = _turtle.p;

		// Move turtle
		_turtle.p.x += step * _scale * cos(_turtle.dir);
		_turtle.p.y += step * _scale * sin(_turtle.dir);

		newLine.b = _turtle.p;
		newLine.c = _color;
		newLine.width = _width;

		// Add to vector
		_trails.insert(_trails.end(), newLine);
		
	}

	void move(double step){
		// Move turtle
		_turtle.p.x += step * _scale * cos(_turtle.dir);
		_turtle.p.y += step * _scale * sin(_turtle.dir);
	}

	void turn(double ang){
		_turtle.dir += ang;
	}

	void resize(double s){
		_scale *= s;
	}

	void color(double r, double g, double b){
		_color.r = r;
		_color.g = g;
		_color.b = b;
	}

	void lineSize(double size){
		_width = size;
	}

	Pt pos() { return _turtle.p; }
}; 

}
#endif