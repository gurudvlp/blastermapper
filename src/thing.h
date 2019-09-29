#include <stdlib.h>
#include <stdio.h>
#include <GL/gl.h>
#include <GL/glx.h>
#include <GL/glu.h>

#include "leveltypes.h"

#ifndef DEF_THING
	#define DEF_THING 1
	#define THING_MAX 512


	int thingFindAt(Level * level, unsigned char x, unsigned char y);

#endif
