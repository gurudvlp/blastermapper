#ifndef HEADER_VIEW_MAP
#define HEADER_VIEW_MAP 1

#include <stdlib.h>
#include <stdio.h>

#include "palette.h"


void RenderMap(int lvl, int lvlmode, int selectedx, int selectedy, unsigned char darken);
void RenderMap_ToggleEditor();
void RenderMap_SelectXLeft();
void RenderMap_SelectXRight();
void RenderMap_SelectYDown();
void RenderMap_SelectYUp();

#endif