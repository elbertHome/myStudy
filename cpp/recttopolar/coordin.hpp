#ifndef COORDIN_H
#define COORDIN_H

struct polar
{
	double distance;	//distance from the orgin
	double angle;		//direction from the orgin
};

struct rect
{
	double x;			// position x
	double y;			// position y
};

//prototypes of functions
polar rect_to_polar(rect pos);
void show_polar(polar pos);

#endif

