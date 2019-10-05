#include <stdlib.h>
#include <stdio.h>
#include <X11/X.h>
#include <X11/Xlib.h>
#include <GL/gl.h>
#include <GL/glx.h>
#include <GL/glu.h>

#include "view_map.h"
#include "../level/leveltypes.h"
#include "../level/levelinfo.h"

void RenderMap_Block(Level * level, BlockID blkid, int x, int y, unsigned char darken);
void RenderMap_SubBlock(Level * level, SubBlockID sblkid, int x, int y);
void RenderMap_UltraSubBlock(Level * level, UltraSubBlockID usblkid, int x, int y);
void RenderMap_SpawnPoint(Level * level);
void DarkenAllQuandrantsBut(int selectedx, int selectedy);
void QuadScreenCoords(int x, int y, float * sx, float * sy, float * ex, float * ey);

int editorVisible = 0;
unsigned short editorSelectX = 0;
unsigned short editorSelectY = 0;

void RenderMap(int lvl, int lvlmode, int selectedx, int selectedy, unsigned char darken)
{
	//	Render the entire world map
	
	
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
	
	for(y = 0; y < 32; y++)
	{
		for(x = 0; x < 32; x++)
		{
			BlockID blkid = (*level).Map[x][y];
			
			if(darken == 0x00 || (x == selectedx && y == selectedy)) { RenderMap_Block(level, blkid, x, 31 - y, 0x00); }
			else { RenderMap_Block(level, blkid, x, 31 - y, 0x01); }
		}
	}
	
	if(darken != 0x00) { DarkenAllQuandrantsBut(selectedx, selectedy); }
	RenderMap_SpawnPoint(level);
}

void RenderMap_Block(Level * level, BlockID blkid, int x, int y, unsigned char darken)
{
	//	x & y represent the position on the 32x32 map grid, NOT SCREEN COORDS
	
	
	SubBlockID subblocks[4];
	subblocks[0] = (*level).Blocks[blkid][0];
	subblocks[1] = (*level).Blocks[blkid][1];
	subblocks[2] = (*level).Blocks[blkid][2];
	subblocks[3] = (*level).Blocks[blkid][3];
	
	RenderMap_SubBlock(level, subblocks[2], x * 2, y * 2);
	RenderMap_SubBlock(level, subblocks[3], (x * 2) + 1, y * 2);
	RenderMap_SubBlock(level, subblocks[0], x * 2, (y * 2) + 1);
	RenderMap_SubBlock(level, subblocks[1], (x * 2) + 1, (y * 2) + 1);
	
	/*if(darken != 0x00)
	{
		float sx, sy;
	
		//	x / 32 = ?/2
		sx = (2.0f * (float)x) / 32.0f;
		sy = (2.0f * (float)y) / 32.0f;
		
		sx -= 1.0f;
		sy -= 1.0f;
		
		glBindTexture(GL_TEXTURE_2D, darkenTextureID);
		
		glBegin(GL_QUADS);
		
			glTexCoord2f(0, 0); glVertex3f(sx, sy + (2.0f / 32.0f), 0.1);
			glTexCoord2f(1, 0); glVertex3f(sx + (2.0f / 32.0f), sy + (2.0f / 32.0f), 0.1);
			glTexCoord2f(1, 1); glVertex3f(sx + (2.0f / 32.0f),  sy, 0.1);
			glTexCoord2f(0, 1); glVertex3f(sx,  sy, 0.1);
		glEnd();
	}*/
}

void RenderMap_SubBlock(Level * level, SubBlockID sblkid, int x, int y)
{
	//	x & y represent position on 64x64 map
	UltraSubBlockID usbs[4];
	usbs[0] = (*level).SubBlocks[sblkid][0];
	usbs[1] = (*level).SubBlocks[sblkid][1];
	usbs[2] = (*level).SubBlocks[sblkid][2];
	usbs[3] = (*level).SubBlocks[sblkid][3];
	
	RenderMap_UltraSubBlock(level, usbs[2], x * 2, y * 2);
	RenderMap_UltraSubBlock(level, usbs[3], (x * 2) + 1, y * 2);
	RenderMap_UltraSubBlock(level, usbs[0], x * 2, (y * 2) + 1);
	RenderMap_UltraSubBlock(level, usbs[1], (x * 2) + 1, (y * 2) + 1);
}

void RenderMap_UltraSubBlock(Level * level, UltraSubBlockID usblkid, int x, int y)
{
	//	x & y represent position on 128x128 map
	float sx, sy;
	
	//	x / 128 = ?/2
	sx = (2.0f * (float)x) / 128.0f;
	sy = (2.0f * (float)y) / 128.0f;
	
	sx -= 1.0f;
	sy -= 1.0f;
	
	//printf("Rendering USB %02x %d,%d %f,%f %d\n", usblkid, x, y, sx, sy,
	//	(*level).USBTextures[usblkid].texid);
	
	glBindTexture(GL_TEXTURE_2D, (*level).USBTextures[usblkid].texid);
	
	glBegin(GL_QUADS);
		
		glTexCoord2f(0, 0); glVertex3f(sx, sy + (2.0f / 128.0f), 0.);
		glTexCoord2f(1, 0); glVertex3f(sx + (2.0f / 128.0f), sy + (2.0f / 128.0f), 0.);
		glTexCoord2f(1, 1); glVertex3f(sx + (2.0f / 128.0f),  sy, 0.);
		glTexCoord2f(0, 1); glVertex3f(sx,  sy, 0.);
	glEnd();
	
	/*glBegin(GL_QUADS);
		
		glTexCoord2f(0, 0); glVertex3f(-.75, -.75, 0.);
		glTexCoord2f(1, 0); glVertex3f(.75, -.75, 0.);
		glTexCoord2f(1, 1); glVertex3f(.75, .75, 0.);
		glTexCoord2f(0, 1); glVertex3f(-.75, .75, 0.);
	glEnd();*/
}

void RenderMap_ToggleEditor()
{
	if(editorVisible == 0) { editorVisible = 1; } else { editorVisible = 0; }
}

void RenderMap_SelectXLeft()
{
	if(editorSelectX == 0) { editorSelectX = 1; }
	else { editorSelectX--; }
}

void RenderMap_SelectXRight()
{
	if(editorSelectX == 1) { editorSelectX = 0; }
	else { editorSelectX++; }
}

void RenderMap_SelectYUp()
{
	if(editorSelectY == 0) { editorSelectY = 1; }
	else { editorSelectY--; }
}

void RenderMap_SelectYDown()
{
	if(editorSelectY == 1) { editorSelectY = 0; }
	else { editorSelectY++; }
}

void DarkenAllQuandrantsBut(int selectedx, int selectedy)
{
	int x, y;
	for(y = 0; y < 2; y++)
	{
		for(x = 0; x < 2; x++)
		{
			if(x != selectedx || y != selectedy)
			{
				float sx, sy, ex, ey;
				QuadScreenCoords(x, y, &sx, &sy, &ex, &ey);
				//printf("DarkenAllQuadrants:\n");
				//printf("\t (%d, %d) -> %f, %f  %f, %f\n", x, y, sx, sy, ex, ey);
				
				//	x0 y0 == x -1, y 1
				//	x0 y1 == x -1 y -1
				//	x1 y0 == x 0, y 1
				
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

void QuadScreenCoords(int x, int y, float * sx, float * sy, float * ex, float * ey)
{
	*sx = (float)x - 1.;
	*ex = *sx + 1.;
	
	*sy = 1. - (float)y;
	*ey = *sy - 1.;
}

void RenderMap_SpawnPoint(Level * level)
{
	unsigned short x = level->SpawnPoint.x;
	unsigned short y = level->SpawnPoint.y;
	
	float sx, sy, ex, ey;
	//	sx/2 = x/128
	sx = (2.0 * (float)x) / 128.0;
	sx -= 1.0;
	ex = sx + (2.0 / 128.0);
	
	sy = (2.0 * (127.0 - (float)y)) / 128.0;
	sy -= 1.0;
	ey = sy - (2.0 / 128.0);
	
	
	
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
		
		sx = (2.0 * (float)x) / 128.0;
		sx -= 1.0;
		ex = sx + (2.0 / 128.0);
		
		sy = (2.0 * (127.0 - (float)y)) / 128.0;
		sy -= 1.0;
		ey = sy + (2.0 / 128.0);
		
		
		
		glBindTexture(GL_TEXTURE_2D, thingSpawnTextureID);
			
		glBegin(GL_QUADS);
		
			glTexCoord2f(0, 0); glVertex3f(sx, ey, 0.11);
			glTexCoord2f(1, 0); glVertex3f(ex, ey, 0.11);
			glTexCoord2f(1, 1); glVertex3f(ex,  sy, 0.11);
			glTexCoord2f(0, 1); glVertex3f(sx,  sy, 0.11);
		glEnd();
	}
}
