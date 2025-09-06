#ifndef HEADER_LEVEL_THING
#define HEADER_LEVEL_THING 1

#include <stdlib.h>
#include <stdio.h>
#include <GL/gl.h>
#include <GL/glx.h>
#include <GL/glu.h>



#ifndef DEF_THING
	#define DEF_THING 1
	
	#include "leveltypes.h"
	
	#define THING_MAX 512


	int thingFindAt(Level * level, unsigned char x, unsigned char y);

#endif

#endif