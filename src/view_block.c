#include <stdlib.h>
#include <stdio.h>
#include <X11/X.h>
#include <X11/Xlib.h>
#include <GL/gl.h>
#include <GL/glx.h>
#include <GL/glu.h>

#include "view_block.h"
#include "leveltypes.h"

void RenderBlock_Block(Level * level, BlockID blkid, int x, int y);
void RenderBlock_SubBlock(Level * level, SubBlockID sblkid, int x, int y);
void RenderBlock_UltraSubBlock(Level * level, UltraSubBlockID usblkid, int x, int y);

void RenderBlock(int lvl, int lvlmode, int block)
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
	
	initx = block / 32;
	endx = initx + 32;
	inity = block % 32;
	endy = inity + 32;
	
	for(y = inity; y < endy; y++)
	{
		for(x = initx; x < endx; x++)
		{
			BlockID blkid = (*level).Map[x][y];
			int yat = y - inity;
			if(yat < 0) { yat += 512; }
			RenderBlock_Block(level, blkid, 0, 0);
		}
	}
	
	
}

void RenderBlock_Block(Level * level, BlockID blkid, int x, int y)
{
	//	x & y represent the position on the 1x1 map grid, NOT SCREEN COORDS
	SubBlockID subblocks[4];
	subblocks[0] = (*level).Blocks[blkid][0];
	subblocks[1] = (*level).Blocks[blkid][1];
	subblocks[2] = (*level).Blocks[blkid][2];
	subblocks[3] = (*level).Blocks[blkid][3];
	
	RenderBlock_SubBlock(level, subblocks[0], x * 2, y * 2);
	RenderBlock_SubBlock(level, subblocks[1], (x * 2) + 1, y * 2);
	RenderBlock_SubBlock(level, subblocks[2], x * 2, (y * 2) + 1);
	RenderBlock_SubBlock(level, subblocks[3], (x * 2) + 1, (y * 2) + 1);
}

void RenderBlock_SubBlock(Level * level, SubBlockID sblkid, int x, int y)
{
	//	x & y represent position on 2x2 map
	UltraSubBlockID usbs[4];
	usbs[0] = (*level).SubBlocks[sblkid][0];
	usbs[1] = (*level).SubBlocks[sblkid][1];
	usbs[2] = (*level).SubBlocks[sblkid][2];
	usbs[3] = (*level).SubBlocks[sblkid][3];
	
	RenderBlock_UltraSubBlock(level, usbs[0], x * 2, y * 2);
	RenderBlock_UltraSubBlock(level, usbs[1], (x * 2) + 1, y * 2);
	RenderBlock_UltraSubBlock(level, usbs[2], x * 2, (y * 2) + 1);
	RenderBlock_UltraSubBlock(level, usbs[3], (x * 2) + 1, (y * 2) + 1);
}

void RenderBlock_UltraSubBlock(Level * level, UltraSubBlockID usblkid, int x, int y)
{
	//	x & y represent position on 4x4 map
	float sx, sy;
	
	//	x / 128 = ?/2
	sx = (2.0f * (float)x) / 4.0f;
	sy = (2.0f * (float)y) / 4.0f;
	
	sx -= 1.0f;
	sy -= 1.0f;
	
	//printf("Rendering USB %02x %d,%d %f,%f %d\n", usblkid, x, y, sx, sy,
	//	(*level).USBTextures[usblkid].texid);
	
	glBindTexture(GL_TEXTURE_2D, (*level).USBTextures[usblkid].texid);
	
	glBegin(GL_QUADS);
		
		glTexCoord2f(0, 0); glVertex3f(sx, sy + (2.0f / 4.0f), 0.);
		glTexCoord2f(1, 0); glVertex3f(sx + (2.0f / 4.0f), sy + (2.0f / 4.0f), 0.);
		glTexCoord2f(1, 1); glVertex3f(sx + (2.0f / 4.0f),  sy, 0.);
		glTexCoord2f(0, 1); glVertex3f(sx,  sy, 0.);
	glEnd();
	
	/*glBegin(GL_QUADS);
		
		glTexCoord2f(0, 0); glVertex3f(-.75, -.75, 0.);
		glTexCoord2f(1, 0); glVertex3f(.75, -.75, 0.);
		glTexCoord2f(1, 1); glVertex3f(.75, .75, 0.);
		glTexCoord2f(0, 1); glVertex3f(-.75, .75, 0.);
	glEnd();*/
}
