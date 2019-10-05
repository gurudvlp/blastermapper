#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <X11/X.h>
#include <X11/Xlib.h>
#include <GL/gl.h>
#include <GL/glx.h>
#include <GL/glu.h>

#include "thing.h"

//	Loop through the Things on this level, and return one that is on this
//	ultra sub block.  If there isn't one, then return -1, which indicates
//	that the thing list is over.
int thingFindAt(Level * level, unsigned char x, unsigned char y)
{
	int et;
	for(et = 0; et < THING_MAX; et++)
	{
		if(level->Things[et].x == x
		&& level->Things[et].y == y)
		{
			return et;
		}
	}
	
	return -1;
}
