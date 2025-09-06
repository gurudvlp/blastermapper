#ifndef HEADER_VIEW_SCREEN
#define HEADER_VIEW_SCREEN 1

#include <stdlib.h>
#include <stdio.h>

#include "palette.h"

unsigned char selectedBlockX;
unsigned char selectedBlockY;

void RenderScreen(int lvl, int lvlmode, int screen, int selectedx, int selectedy, unsigned char darken);

#endif