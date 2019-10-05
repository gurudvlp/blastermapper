#include <stdlib.h>
#include <stdio.h>
#include <X11/X.h>
#include <X11/Xlib.h>
#include <GL/gl.h>
#include <GL/glx.h>
#include <GL/glu.h>

#include "view_quadrant.h"
#include "view_screen.h"
#include "../level/leveltypes.h"
#include "../level/levelinfo.h"

void RenderScreen_Block(Level * level, BlockID blkid, int x, int y);
void RenderScreen_SubBlock(Level * level, SubBlockID sblkid, int x, int y);
void RenderScreen_UltraSubBlock(Level * level, UltraSubBlockID usblkid, int x, int y);
void RenderScreen_SpawnPoint(Level * level, int screen);

void RenderScreen(int lvl, int lvlmode, int screen, int selectedx, int selectedy, unsigned char darken)
{
	//	Render a screen of the map
	
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
	
	initx = (screen % 8) * 4;
	endx = initx + 4;
	inity = (screen / 8) * 4;
	endy = inity + 4;
	
	
	for(y = inity; y < endy; y++)
	{
		for(x = initx; x < endx; x++)
		{
			BlockID blkid = (*level).Map[x][y];
			int yat = y - inity;
			if(yat < 0) { yat += 256; }
			RenderScreen_Block(level, blkid, x - initx, 3 - yat);
			
			
		}
	}
	
	if(darken != 0x00)
	{
		for(y = 0; y < 8; y++)
		{
			for(x = 0; x < 8; x++)
			{
				if(y == selectedy && x == selectedx)
				{
					//	selected screen, don't darken
					selectedBlockX = initx + x;
					selectedBlockY = inity + y;
				}
				else
				{
					//	darken this screen
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
	}
	
	RenderScreen_SpawnPoint(level, screen);
}

void RenderScreen_Block(Level * level, BlockID blkid, int x, int y)
{
	//	x & y represent the position on the 8x8 map grid, NOT SCREEN COORDS
	SubBlockID subblocks[4];
	subblocks[0] = (*level).Blocks[blkid][0];
	subblocks[1] = (*level).Blocks[blkid][1];
	subblocks[2] = (*level).Blocks[blkid][2];
	subblocks[3] = (*level).Blocks[blkid][3];
	
	RenderScreen_SubBlock(level, subblocks[2], x * 2, y * 2);
	RenderScreen_SubBlock(level, subblocks[3], (x * 2) + 1, y * 2);
	RenderScreen_SubBlock(level, subblocks[0], x * 2, (y * 2) + 1);
	RenderScreen_SubBlock(level, subblocks[1], (x * 2) + 1, (y * 2) + 1);
}

void RenderScreen_SubBlock(Level * level, SubBlockID sblkid, int x, int y)
{
	//	x & y represent position on 16x16 map
	UltraSubBlockID usbs[4];
	usbs[0] = (*level).SubBlocks[sblkid][0];
	usbs[1] = (*level).SubBlocks[sblkid][1];
	usbs[2] = (*level).SubBlocks[sblkid][2];
	usbs[3] = (*level).SubBlocks[sblkid][3];
	
	RenderScreen_UltraSubBlock(level, usbs[2], x * 2, y * 2);
	RenderScreen_UltraSubBlock(level, usbs[3], (x * 2) + 1, y * 2);
	RenderScreen_UltraSubBlock(level, usbs[0], x * 2, (y * 2) + 1);
	RenderScreen_UltraSubBlock(level, usbs[1], (x * 2) + 1, (y * 2) + 1);
}

void RenderScreen_UltraSubBlock(Level * level, UltraSubBlockID usblkid, int x, int y)
{
	//	x & y represent position on 32x32 map
	float sx, sy;
	
	//	x / 128 = ?/2
	sx = (2.0f * (float)x) / 16.0f;
	sy = (2.0f * (float)y) / 16.0f;
	
	sx -= 1.0f;
	sy -= 1.0f;
	
	//printf("Rendering USB %02x %d,%d %f,%f %d\n", usblkid, x, y, sx, sy,
	//	(*level).USBTextures[usblkid].texid);
	
	glBindTexture(GL_TEXTURE_2D, (*level).USBTextures[usblkid].texid);
	
	glBegin(GL_QUADS);
		
		glTexCoord2f(0, 0); glVertex3f(sx, sy + (2.0f / 16.0f), 0.);
		glTexCoord2f(1, 0); glVertex3f(sx + (2.0f / 16.0f), sy + (2.0f / 16.0f), 0.);
		glTexCoord2f(1, 1); glVertex3f(sx + (2.0f / 16.0f),  sy, 0.);
		glTexCoord2f(0, 1); glVertex3f(sx,  sy, 0.);
	glEnd();
	
	/*glBegin(GL_QUADS);
		
		glTexCoord2f(0, 0); glVertex3f(-.75, -.75, 0.);
		glTexCoord2f(1, 0); glVertex3f(.75, -.75, 0.);
		glTexCoord2f(1, 1); glVertex3f(.75, .75, 0.);
		glTexCoord2f(0, 1); glVertex3f(-.75, .75, 0.);
	glEnd();*/
}

void RenderScreen_SpawnPoint(Level * level, int screen)
{
	unsigned short x = level->SpawnPoint.x;
	unsigned short y = level->SpawnPoint.y;
	
	int scx = screen % 8;
	int scy = screen / 8;
	
	float sx, sy, ex, ey;
	//	sx/2 = x/128
	sx = (2.0 * (float)x) / 16.0;
	sx -= 1.0;
	//if(quadrant == 1 || quadrant == 3) { sx -= 2.; }
	sx -= ((float)scx * 2.);
	
	sy = (2.0 * (31.0 - (float)y)) / 16.0;
	sy -= 3.0;
	//if(quadrant == 2 || quadrant == 3) { sy += 2.; }
	sy += ((float)scy * 2.);
	
	ex = sx + (2.0 / 16.0);
	ey = sy - (2.0 / 16.0);
	
	
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
		
		scx = screen % 8;
		scy = screen / 8;
		
		//	sx/2 = x/128
		sx = (2.0 * (float)x) / 16.0;
		sx -= 1.0;
		//if(quadrant == 1 || quadrant == 3) { sx -= 2.; }
		sx -= ((float)scx * 2.);
		
		sy = (2.0 * (31.0 - (float)y)) / 16.0;
		sy -= 3.0;
		//if(quadrant == 2 || quadrant == 3) { sy += 2.; }
		sy += ((float)scy * 2.);
		
		ex = sx + (2.0 / 16.0);
		ey = sy + (2.0 / 16.0);
		
		
		glBindTexture(GL_TEXTURE_2D, thingSpawnTextureID);
			
		glBegin(GL_QUADS);
		
			glTexCoord2f(0, 0); glVertex3f(sx, ey, 0.11);
			glTexCoord2f(1, 0); glVertex3f(ex, ey, 0.11);
			glTexCoord2f(1, 1); glVertex3f(ex,  sy, 0.11);
			glTexCoord2f(0, 1); glVertex3f(sx,  sy, 0.11);
		glEnd();
	}
}
