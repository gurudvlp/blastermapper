#include <stdlib.h>
#include <stdio.h>
#include <X11/X.h>
#include <X11/Xlib.h>
#include <GL/gl.h>
#include <GL/glx.h>
#include <GL/glu.h>

#include "view_quadrant.h"
#include "../level/leveltypes.h"
#include "../level/levelinfo.h"

void RenderQuadrant_Block(Level * level, BlockID blkid, int x, int y);
void RenderQuadrant_SubBlock(Level * level, SubBlockID sblkid, int x, int y);
void RenderQuadrant_UltraSubBlock(Level * level, UltraSubBlockID usblkid, int x, int y);
void RenderQuadrant_SpawnPoint(Level * level, int quadrant);


void RenderQuadrant(int lvl, int lvlmode, int quadrant, int selectedx, int selectedy, unsigned char darken)
{
	//	Render a quadrant of the map
	
	Level * level = (Level *)&Levels[lvl][lvlmode];
	
	glClearColor(1.0, 1.0, 1.0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(-1., 1., -1., 1., 1., 20.);
	
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(0., 0., 10., 0., 0., 0., 0., 1., 0.);
	
	int x, y;
	int initx, inity, endx, endy;
	
	if(quadrant == 0)
	{
		initx = 0;
		inity = 0;
		endx = 16;
		endy = 16;
	}
	else if(quadrant == 1)
	{
		initx = 16;
		inity = 0;
		endx = 32;
		endy = 16;
	}
	else if(quadrant == 2)
	{
		initx = 0;
		inity = 16;
		endx = 16;
		endy = 32;
	}
	else
	{
		initx = 16;
		inity = 16;
		endx = 32;
		endy = 32;
	}
	

	
	for(y = inity; y < endy; y++)
	{
		for(x = initx; x < endx; x++)
		{
			
			
			BlockID blkid = (*level).Map[x][y];
			int yat = y - inity;
			if(yat < 0) { yat += 64; }
			RenderQuadrant_Block(level, blkid, x - initx, 15 - yat);
			
			
			
			//if(initx - x == selectedx && inity - y == selectedy)
			/*if(scrx == selectedx && scry == selectedy)
			{
				//	this is the selected screen
			}
			else
			{
				//	darken
				//printf("View_Quadrant: Darken: %d, %d\n", x, y);
				float sx, sy, ex, ey;
				sx = (float)initx - (float)x;
				sy = (float)inity - (float)y;
				
				sy = 15 - sy;
				
				sx = (sx * 2.0) / 32.0;
				sy = (sy * 2.0) / 32.0;
				sx--;
				sy--;
				
				//ex = sx + 1.0;
				//ey = sy - 1.0;
				ex = sx + (2. / 32.);
				ey = sy + (2. / 32.);
				
				glBindTexture(GL_TEXTURE_2D, darkenTextureID);
		
				glBegin(GL_QUADS);
				
					glTexCoord2f(0, 0); glVertex3f(sx, ey, 0.1);
					glTexCoord2f(1, 0); glVertex3f(ex, ey, 0.1);
					glTexCoord2f(1, 1); glVertex3f(ex,  sy, 0.1);
					glTexCoord2f(0, 1); glVertex3f(sx,  sy, 0.1);
				glEnd();
			}*/
		}
	}
	
	for(y = 0; y < 4; y++)
	{
		for(x = 0; x < 4; x++)
		{
			if(y == selectedy && x == selectedx)
			{
				//	currently selected screen
				//printf("Found selected x,y %d, %d\n", x, y);
			}
			else
			{
				//	darken this screen
				//	testx/4 == ?/2.0
				
				float sx, sy, ex, ey;
				sx = ((float)x * 2.0) / 4.0;
				sy = 4.0 - (float)y;
				sy = (sy * 2.0) / 4.0;
				
				
				sx--;
				sy--;
				
				//printf("Darken %f, %f\n", sx, sy);
				
				ex = sx + (0.5);
				ey = sy - (0.5);
				
				glBindTexture(GL_TEXTURE_2D, darkenTextureID);
		
				glBegin(GL_QUADS);
				
					glTexCoord2f(0, 0); glVertex3f(sx, ey, 0.1);
					glTexCoord2f(1, 0); glVertex3f(ex, ey, 0.1);
					glTexCoord2f(1, 1); glVertex3f(ex,  sy, 0.1);
					glTexCoord2f(0, 1); glVertex3f(sx,  sy, 0.1);
				glEnd();
			}
		}
	}
	
	RenderQuadrant_SpawnPoint(level, quadrant);
}

void RenderQuadrant_Block(Level * level, BlockID blkid, int x, int y)
{
	//	x & y represent the position on the 16x16 map grid, NOT SCREEN COORDS
	SubBlockID subblocks[4];
	subblocks[0] = (*level).Blocks[blkid][0];
	subblocks[1] = (*level).Blocks[blkid][1];
	subblocks[2] = (*level).Blocks[blkid][2];
	subblocks[3] = (*level).Blocks[blkid][3];
	
	RenderQuadrant_SubBlock(level, subblocks[2], x * 2, y * 2);
	RenderQuadrant_SubBlock(level, subblocks[3], (x * 2) + 1, y * 2);
	RenderQuadrant_SubBlock(level, subblocks[0], x * 2, (y * 2) + 1);
	RenderQuadrant_SubBlock(level, subblocks[1], (x * 2) + 1, (y * 2) + 1);
}

void RenderQuadrant_SubBlock(Level * level, SubBlockID sblkid, int x, int y)
{
	//	x & y represent position on 32x32 map
	UltraSubBlockID usbs[4];
	usbs[0] = (*level).SubBlocks[sblkid][0];
	usbs[1] = (*level).SubBlocks[sblkid][1];
	usbs[2] = (*level).SubBlocks[sblkid][2];
	usbs[3] = (*level).SubBlocks[sblkid][3];
	
	RenderQuadrant_UltraSubBlock(level, usbs[2], x * 2, y * 2);
	RenderQuadrant_UltraSubBlock(level, usbs[3], (x * 2) + 1, y * 2);
	RenderQuadrant_UltraSubBlock(level, usbs[0], x * 2, (y * 2) + 1);
	RenderQuadrant_UltraSubBlock(level, usbs[1], (x * 2) + 1, (y * 2) + 1);
}

void RenderQuadrant_UltraSubBlock(Level * level, UltraSubBlockID usblkid, int x, int y)
{
	//	x & y represent position on 64x64 map
	float sx, sy;
	
	//	x / 128 = ?/2
	sx = (2.0f * (float)x) / 64.0f;
	sy = (2.0f * (float)y) / 64.0f;
	
	sx -= 1.0f;
	sy -= 1.0f;
	
	//printf("Rendering USB %02x %d,%d %f,%f %d\n", usblkid, x, y, sx, sy,
	//	(*level).USBTextures[usblkid].texid);
	
	glBindTexture(GL_TEXTURE_2D, (*level).USBTextures[usblkid].texid);
	
	glBegin(GL_QUADS);
		
		glTexCoord2f(0, 0); glVertex3f(sx, sy + (2.0f / 64.0f), 0.);
		glTexCoord2f(1, 0); glVertex3f(sx + (2.0f / 64.0f), sy + (2.0f / 64.0f), 0.);
		glTexCoord2f(1, 1); glVertex3f(sx + (2.0f / 64.0f),  sy, 0.);
		glTexCoord2f(0, 1); glVertex3f(sx,  sy, 0.);
		
		
	glEnd();
	
	/*glBegin(GL_QUADS);
		
		glTexCoord2f(0, 0); glVertex3f(-.75, -.75, 0.);
		glTexCoord2f(1, 0); glVertex3f(.75, -.75, 0.);
		glTexCoord2f(1, 1); glVertex3f(.75, .75, 0.);
		glTexCoord2f(0, 1); glVertex3f(-.75, .75, 0.);
	glEnd();*/
}


void RenderQuadrant_SpawnPoint(Level * level, int quadrant)
{
	unsigned short x = level->SpawnPoint.x;
	unsigned short y = level->SpawnPoint.y;
	
	float sx, sy, ex, ey;
	//	sx/2 = x/128
	sx = (2.0 * (float)x) / 64.0;
	sx -= 1.0;
	if(quadrant == 1 || quadrant == 3) { sx -= 2.; }
	
	sy = (2.0 * (127.0 - (float)y)) / 64.0;
	sy -= 3.0;
	if(quadrant == 2 || quadrant == 3) { sy += 2.; }
	
	ex = sx + (2.0 / 64.0);
	ey = sy - (2.0 / 64.0);
	
	
	//printf("RenderQuadrant: SpawnPoint at (%f, %f) - (%f, %f)\n", sx, sy, ex, ey);
	
	glBindTexture(GL_TEXTURE_2D, spawnPointTextureID);
		
	glBegin(GL_QUADS);
	
		glTexCoord2f(0, 0); glVertex3f(sx, ey, 0.11);
		glTexCoord2f(1, 0); glVertex3f(ex, ey, 0.11);
		glTexCoord2f(1, 1); glVertex3f(ex,  sy, 0.11);
		glTexCoord2f(0, 1); glVertex3f(sx,  sy, 0.11);
	glEnd();
	
	int et;
	for(et = 0; et < 256; et++)
	{
		if(level->Things[et].thingtype == 0xFF) { break; }
		
		x = level->Things[et].x;
		y = level->Things[et].y;
		
		sx = (2.0 * (float)x) / 64.0;
		sx -= 1.0;
		if(quadrant == 1 || quadrant == 3) { sx -= 2.; }
		
		sy = (2.0 * (127.0 - (float)y)) / 64.0;
		sy -= 3.0;
		if(quadrant == 2 || quadrant == 3) { sy += 2.; }
		
		ex = sx + (2.0 / 64.0);
		ey = sy + (2.0 / 64.0);
		
		
		//printf("RenderQuadrant: SpawnPoint at (%f, %f) - (%f, %f)\n", sx, sy, ex, ey);
		
		glBindTexture(GL_TEXTURE_2D, thingSpawnTextureID);
			
		glBegin(GL_QUADS);
		
			glTexCoord2f(0, 0); glVertex3f(sx, ey, 0.11);
			glTexCoord2f(1, 0); glVertex3f(ex, ey, 0.11);
			glTexCoord2f(1, 1); glVertex3f(ex,  sy, 0.11);
			glTexCoord2f(0, 1); glVertex3f(sx,  sy, 0.11);
		glEnd();
	}
}
