/*
 * main.c
 * 
 * Copyright 2016 Brian Murphy <guru@gurutronik.com>
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301, USA.
 * 
 * 
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <X11/X.h>
#include <X11/Xlib.h>
#include <GL/gl.h>
#include <GL/glx.h>
#include <GL/glu.h>
#include <unistd.h>
#include <sys/time.h>

//#include "leveltypes.h"
#include "main.h"
#include "loadrom.h"
#include "saverom.h"
#include "palette.h"
#include "view_map.h"
#include "view_quadrant.h"
#include "view_screen.h"
#include "view_block.h"
#include "levelinfo.h"
//#include "thing.h"

void SetupXWindows();
void SetupGL();
void DrawAQuad();

void GetCmdEditorCoords(int argc, char ** argv);

void Selecter_MoveDown();
void Selecter_MoveLeft();
void Selecter_MoveRight();
void Selecter_MoveUp();
void RenderEditor();

Display *dpy;
Window root;
GLint att[] = { GLX_RGBA, GLX_DEPTH_SIZE, 24, GLX_DOUBLEBUFFER, None };
XVisualInfo *vi;
Colormap cmap;
XSetWindowAttributes swa;
Window win;
GLXContext glc;
XWindowAttributes gwa;
XEvent xev;

//	Temporary texture testing stuff
void CreateTestTexture();
GLuint testtexid;
GLubyte testTex[8 * 8 * 4];
GLubyte darkenTex[8 * 8 * 4];
GLubyte spawnPointTex[8 * 8 * 4];
GLubyte thingSpawnTex[8 * 8 * 4];

#define ZOOM_MAP 0
#define ZOOM_QUADRANT 1
#define ZOOM_SCREEN 2
#define ZOOM_BLOCK 3

int editorZoom = ZOOM_MAP;
int editorLevel = 0;
unsigned char editorMode = 0x00;
unsigned char editorQuadrant = 0x00;
unsigned short editorX = 0;
unsigned short editorY = 0;
unsigned short editorXSelect = 0;
unsigned short editorYSelect = 0;
unsigned short editorCoords[4][4];

int main(int argc, char **argv)
{
	printf("blaster mapper v0.0.2\n");
	
	if(argc < 2)
	{
		printf("Please specify the ROM file to open.\n");
		exit(0);
	}
	
	InitializePalette();
	
	if(!LoadRom(argv[1]))
	{
		printf("Failed to load ROM.\n");
		exit(0);
	}
	
	
	
	if(IsCmdOptionSet(argc, argv, "things10")) { PrintThings(0, 0x00); }
	if(IsCmdOptionSet(argc, argv, "things11")) { PrintThings(0, 0x01); }
	if(IsCmdOptionSet(argc, argv, "things20")) { PrintThings(1, 0x00); }
	if(IsCmdOptionSet(argc, argv, "things21")) { PrintThings(1, 0x01); }
	if(IsCmdOptionSet(argc, argv, "things30")) { PrintThings(2, 0x00); }
	if(IsCmdOptionSet(argc, argv, "things31")) { PrintThings(2, 0x01); }
	if(IsCmdOptionSet(argc, argv, "things40")) { PrintThings(3, 0x00); }
	if(IsCmdOptionSet(argc, argv, "things41")) { PrintThings(3, 0x01); }
	if(IsCmdOptionSet(argc, argv, "things50")) { PrintThings(4, 0x00); }
	if(IsCmdOptionSet(argc, argv, "things51")) { PrintThings(4, 0x01); }
	if(IsCmdOptionSet(argc, argv, "things60")) { PrintThings(5, 0x00); }
	if(IsCmdOptionSet(argc, argv, "things61")) { PrintThings(5, 0x01); }
	if(IsCmdOptionSet(argc, argv, "things70")) { PrintThings(6, 0x00); }
	if(IsCmdOptionSet(argc, argv, "things71")) { PrintThings(6, 0x01); }
	if(IsCmdOptionSet(argc, argv, "things80")) { PrintThings(7, 0x00); }
	if(IsCmdOptionSet(argc, argv, "things81")) { PrintThings(7, 0x01); }
	
	if(IsCmdOptionSet(argc, argv, "level10")) { editorLevel = 0; editorMode = 0; }
	if(IsCmdOptionSet(argc, argv, "level11")) { editorLevel = 0; editorMode = 1; }
	if(IsCmdOptionSet(argc, argv, "level20")) { editorLevel = 1; editorMode = 0; }
	if(IsCmdOptionSet(argc, argv, "level21")) { editorLevel = 1; editorMode = 1; }
	if(IsCmdOptionSet(argc, argv, "level30")) { editorLevel = 2; editorMode = 0; }
	if(IsCmdOptionSet(argc, argv, "level31")) { editorLevel = 2; editorMode = 1; }
	if(IsCmdOptionSet(argc, argv, "level40")) { editorLevel = 3; editorMode = 0; }
	if(IsCmdOptionSet(argc, argv, "level41")) { editorLevel = 3; editorMode = 1; }
	if(IsCmdOptionSet(argc, argv, "level50")) { editorLevel = 4; editorMode = 0; }
	if(IsCmdOptionSet(argc, argv, "level51")) { editorLevel = 4; editorMode = 1; }
	if(IsCmdOptionSet(argc, argv, "level60")) { editorLevel = 5; editorMode = 0; }
	if(IsCmdOptionSet(argc, argv, "level61")) { editorLevel = 5; editorMode = 1; }
	if(IsCmdOptionSet(argc, argv, "level70")) { editorLevel = 6; editorMode = 0; }
	if(IsCmdOptionSet(argc, argv, "level71")) { editorLevel = 6; editorMode = 1; }
	if(IsCmdOptionSet(argc, argv, "level80")) { editorLevel = 7; editorMode = 0; }
	if(IsCmdOptionSet(argc, argv, "level81")) { editorLevel = 7; editorMode = 1; }
	
	if(IsCmdOptionSet(argc, argv, "quad0")) { editorQuadrant = 0x00; }
	if(IsCmdOptionSet(argc, argv, "quad1")) { editorQuadrant = 0x01; }
	if(IsCmdOptionSet(argc, argv, "quad2")) { editorQuadrant = 0x02; }
	if(IsCmdOptionSet(argc, argv, "quad3")) { editorQuadrant = 0x03; }
	
	if(IsCmdOptionSet(argc, argv, "zoommap")) { editorZoom = ZOOM_MAP; }
	if(IsCmdOptionSet(argc, argv, "zoomquadrant")) { editorZoom = ZOOM_QUADRANT; }
	if(IsCmdOptionSet(argc, argv, "zoomscreen")) { editorZoom = ZOOM_SCREEN; }
	if(IsCmdOptionSet(argc, argv, "zoomblock")) { editorZoom = ZOOM_BLOCK; }
	
	GetCmdEditorCoords(argc, argv);
	
	if(IsCmdOptionSet(argc, argv, "levelpointers")) { SAVEROM_SHOW_LEVEL_POINTERS = true; }
	else { SAVEROM_SHOW_LEVEL_POINTERS = false; }
	
	if(IsCmdOptionSet(argc, argv, "print-spawn")
	|| IsCmdOptionSet(argc, argv, "print-spawns")
	|| IsCmdOptionSet(argc, argv, "spawnpoints"))
	{
		PrintSpawnPoints();
	}
	
	
	
	if(IsCmdOptionSet(argc, argv, "save")) 
	{
		//	Only save the new rom, and exit
		SaveRom();
		exit(0);
	}
	
	SetupXWindows();
	SetupGL();
	
	printf("Creating textures from CHR ROM\n");
	int el;
	for(el = 0; el < 8; el++)
	{
		LoadUSBTextures(el, 0);
		LoadUSBTextures(el, 1);
	}
	
	//	The heart loop
	while(1)
	{
		while(XPending(dpy))
		{
			
			XNextEvent(dpy, &xev);
		
			if(xev.type == Expose)
			{
				
				XGetWindowAttributes(dpy, win, &gwa);
				glViewport(0, 0, gwa.width, gwa.height);
				
				
				//DrawAQuad();
				/*if(editorZoom == ZOOM_MAP) { RenderMap(editorLevel, editorMode, editorXSelect, editorYSelect, 0x01); }
				else if(editorZoom == ZOOM_QUADRANT) { RenderQuadrant(editorLevel, editorMode, (editorY * 2) + editorX); }
				else if(editorZoom == ZOOM_SCREEN) { RenderScreen(editorLevel, editorMode, (editorY * 8) + editorX); }
				else { RenderBlock(editorLevel, editorMode, (editorY * 32) + editorX); }
				*/
				RenderEditor();
				
				glXSwapBuffers(dpy, win);
				
			}
			else if(xev.type == KeyPress)
			{
				//printf("KeyPress event received.\n");
				
				XKeyEvent * xke = (XKeyEvent *)&xev;
				printf("Keycode %d\n", (*xke).keycode);
				//	up 111
				//	down 116
				//	left 113
				//	right 114
				//	space 65
				//	enter 36
				//	escape 9
				//	numpad8 80
				//	numpad2 88
				//	numpad4 83
				//	numpad6 85
				//	numpad5 84
				//	c 54
				//	v 55
				//	+ 86
				//	= 21
				//	- 82
				//	[ 34
				//	] 35
				//	t 28
				//	keyboard1 10,11,12...
				if(xke->keycode == 9)
				{
					if(editorZoom == 0)
					{
						glXMakeCurrent(dpy, None, NULL);
						glXDestroyContext(dpy, glc);
						XDestroyWindow(dpy, win);
						XCloseDisplay(dpy);
						exit(0);
					}
					else
					{
						
						editorZoom--;
						editorX = editorCoords[editorZoom][0];
						editorY = editorCoords[editorZoom][1];
						editorXSelect = editorCoords[editorZoom][2];
						editorYSelect = editorCoords[editorZoom][3];
						

						RenderEditor();
						
						glXSwapBuffers(dpy, win);
					}
				}
				
				if(xke->keycode == 28)
				{
					//int cThing = thingFindAt(&Levels[editorLevel][editorMode], editorXSelect, editorYSelect);
					//printf("Thing at %d %d == %d\n", editorXSelect, editorYSelect, cThing);
				}
				
				if(xke->keycode == 113)
				{
					//	move selection left
					Selecter_MoveLeft();
					RenderEditor();
				}
				
				if(xke->keycode == 114)
				{
					//	move selection right
					Selecter_MoveRight();
					RenderEditor();
				}
				
				if(xke->keycode == 111)
				{
					//	move selection up
					Selecter_MoveUp();
					RenderEditor();
				}
				
				if(xke->keycode == 116)
				{
					//	move selection down
					Selecter_MoveDown();
					RenderEditor();
				}
				
				if(xke->keycode == 36)
				{
					//	enter, zoom in
					editorCoords[editorZoom][0] = editorX;
					editorCoords[editorZoom][1] = editorY;
					editorCoords[editorZoom][2] = editorXSelect;
					editorCoords[editorZoom][3] = editorYSelect;
					
					editorZoom++;
					if(editorZoom > ZOOM_BLOCK) { editorZoom = ZOOM_BLOCK; }
					else
					{
						if(editorZoom == ZOOM_QUADRANT)
						{
							editorX = editorXSelect;
							editorY = editorYSelect;
						}
						else if(editorZoom == ZOOM_SCREEN)
						{
							if(editorX == 1)
							{
								editorX = 4 + editorXSelect;
							}
							else
							{
								editorX = editorXSelect;
							}
							
							if(editorY == 1)
							{
								editorY = 4 + editorYSelect;
							}
							else
							{
								editorY = editorYSelect;
							}
							
						}
						
						editorXSelect = 0;
						editorYSelect = 0;
						
						RenderEditor();
					}
				}
				
				if(xke->keycode == 39)
				{
					//	s key, save work
					SaveRom();
				}
				
				if(xke->keycode >= 10 && xke->keycode <= 17)
				{
					//	keyboard number key.  switch level
					editorLevel = xke->keycode - 10;
					RenderEditor();
				}
				
				if(xke->keycode == 19)
				{
					//	keyboard 0, switch modes
					if(editorMode == 0) { editorMode = 1; }
					else { editorMode = 0; }
					
					RenderEditor();
				}
				
				if(editorZoom == ZOOM_SCREEN)
				{
					//	Check for block rotation keystrokes
					if(xke->keycode == 80
					|| xke->keycode == 35)
					{
						//	keypad 8, "up"
						//	]
						unsigned char cblock = BlockAt(editorLevel, editorMode, selectedBlockX, selectedBlockY);
						if(cblock == 0xFF) { cblock = 0x00; }
						else if(cblock == GetHighestBlockID(editorLevel, editorMode)) { cblock = 0x00; }
						else { cblock++; }
						
						SetBlockAt(editorLevel, editorMode, selectedBlockX, selectedBlockY, cblock);
						RenderEditor();
					}
					
					if(xke->keycode == 88
					|| xke->keycode == 34)
					{
						//	keypad 2, "down"
						//	[
						unsigned char cblock = BlockAt(editorLevel, editorMode, selectedBlockX, selectedBlockY);
						if(cblock == 0x00) { cblock = GetHighestBlockID(editorLevel, editorMode); }
						else { cblock--; }
						
						SetBlockAt(editorLevel, editorMode, selectedBlockX, selectedBlockY, cblock);
						RenderEditor();
					}
					
					if(xke->keycode == 54)
					{
						//	c, copy block
						BlockClipboard = BlockAt(editorLevel, editorMode, selectedBlockX, selectedBlockY);
					}
					
					if(xke->keycode == 55)
					{
						//	v, paste block
						SetBlockAt(editorLevel, editorMode, selectedBlockX, selectedBlockY, BlockClipboard);
						RenderEditor();
					}
					
					if(xke->keycode == 86
					|| xke->keycode == 21)
					{
						//	+, set spawn point
						//	=
						unsigned short spx = (selectedBlockX * 4) + 1;
						unsigned short spy = (selectedBlockY * 4) + 1;
						SetSpawnPoint(editorLevel, editorMode, spx, spy);
						
						RenderEditor();
					}	
					
					if(xke->keycode == 28)
					{
						unsigned short sbx = (selectedBlockX * 4);
						unsigned short sby = (selectedBlockY * 4);
						int tx, ty;
						for(tx = 0; tx < 4; tx++)
						{
							for(ty = 0; ty < 4; ty++)
							{
								printf("Thing @ %d, %d: %d\n", 
									sbx + tx, 
									sby + ty,
									GetThingAt(
										editorLevel,
										editorMode,
										sbx + tx,
										sby + ty
									)
								);
								
								
							}
						}
					}
				}
			}
		}
		
		sleep(0);
	}
	
	return 0;
}

bool IsCmdOptionSet(int argc, char ** argv, char option[])
{
	int ea;
	for(ea = 0; ea < argc; ea++)
	{
		if(strcmp(argv[ea], option) == 0) { return true; }
	}
	
	return false;
}

void SetupXWindows()
{
	dpy = XOpenDisplay(NULL);
	
	if(dpy == NULL)
	{
		printf("cannot connect to X server :(\n");
		exit(0);
	}
	
	root = DefaultRootWindow(dpy);
	
	vi = glXChooseVisual(dpy, 0, att);
	
	if(vi == NULL)
	{
		printf("no appropriate visual found\n");
		exit(0);
	}
	else
	{
		printf("visual %p selected\n", (void*)vi->visualid);
	}
	
	cmap = XCreateColormap(dpy, root, vi->visual, AllocNone);
	
	swa.colormap = cmap;
	swa.event_mask = ExposureMask | KeyPressMask;
	
	win = XCreateWindow(dpy, root, 0, 0, WindowWidth, WindowHeight, 0, vi->depth, InputOutput, vi->visual, CWColormap | CWEventMask, &swa);
	XMapWindow(dpy, win);
	XStoreName(dpy, win, "blaster mapper");
	
	glc = glXCreateContext(dpy, vi, NULL, GL_TRUE);
	glXMakeCurrent(dpy, win, glc);
	
	glEnable(GL_DEPTH_TEST);
}

void SetupGL()
{
	glClearColor(1.0, 1.0, 1.0, 1.0);
	//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(-1., 1., -1., 1., 1., 20.);
	
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(0., 0., 10., 0., 0., 0., 0., 1., 0.);
	
	glEnable(GL_TEXTURE_2D);
	
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_BLEND);
	
	//	Create the shaded/darken texture
	
	int c;
	for(c = 0; c < (8 * 8); c++)
	{
		darkenTex[c * 4] = 0x00;
		darkenTex[(c * 4) + 1] = 0x00;
		darkenTex[(c * 4) + 2] = 0x00;
		darkenTex[(c * 4) + 3] = 0x80;
	}
	
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	
	glGenTextures(1, &darkenTextureID);
	glBindTexture(GL_TEXTURE_2D, darkenTextureID);
	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, 
					GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, 
					GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 8, 
					8, 0, GL_RGBA, GL_UNSIGNED_BYTE, 
					darkenTex);
					
	//	Create spawn point texture
	
	for(c = 0; c < 8; c++)
	{
		spawnPointTex[(c * 32) + 12] = 0x00;
		spawnPointTex[(c * 32) + 13] = 0xC0;
		spawnPointTex[(c * 32) + 14] = 0x00;
		spawnPointTex[(c * 32) + 15] = 0x80;
		
		spawnPointTex[(c * 32) + 16] = 0x00;
		spawnPointTex[(c * 32) + 17] = 0xC0;
		spawnPointTex[(c * 32) + 18] = 0x00;
		spawnPointTex[(c * 32) + 19] = 0x80;
		
		spawnPointTex[96 + (c * 4)] = 0x00;
		spawnPointTex[96 + (c * 4) + 1] = 0xC0;
		spawnPointTex[96 + (c * 4) + 2] = 0x00;
		spawnPointTex[96 + (c * 4) + 3] = 0x80;
		
		spawnPointTex[128 + (c * 4)] = 0x00;
		spawnPointTex[128 + (c * 4) + 1] = 0xC0;
		spawnPointTex[128 + (c * 4) + 2] = 0x00;
		spawnPointTex[128 + (c * 4) + 3] = 0x80;
	}
	
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	
	glGenTextures(1, &spawnPointTextureID);
	glBindTexture(GL_TEXTURE_2D, spawnPointTextureID);
	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, 
					GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, 
					GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 8, 
					8, 0, GL_RGBA, GL_UNSIGNED_BYTE, 
					spawnPointTex);
	
	//	Create thing spawn point texture
	
	for(c = 0; c < 8; c++)
	{
		thingSpawnTex[(c * 32) + 12] = 0xC0;
		thingSpawnTex[(c * 32) + 13] = 0x00;
		thingSpawnTex[(c * 32) + 14] = 0x00;
		thingSpawnTex[(c * 32) + 15] = 0x80;
		
		thingSpawnTex[(c * 32) + 16] = 0xC0;
		thingSpawnTex[(c * 32) + 17] = 0x00;
		thingSpawnTex[(c * 32) + 18] = 0x00;
		thingSpawnTex[(c * 32) + 19] = 0x80;
		
		thingSpawnTex[96 + (c * 4)] = 0xC0;
		thingSpawnTex[96 + (c * 4) + 1] = 0x00;
		thingSpawnTex[96 + (c * 4) + 2] = 0x00;
		thingSpawnTex[96 + (c * 4) + 3] = 0x80;
		
		thingSpawnTex[128 + (c * 4)] = 0xC0;
		thingSpawnTex[128 + (c * 4) + 1] = 0x00;
		thingSpawnTex[128 + (c * 4) + 2] = 0x00;
		thingSpawnTex[128 + (c * 4) + 3] = 0x80;
	}
	
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	
	glGenTextures(1, &thingSpawnTextureID);
	glBindTexture(GL_TEXTURE_2D, thingSpawnTextureID);
	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, 
					GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, 
					GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 8, 
					8, 0, GL_RGBA, GL_UNSIGNED_BYTE, 
					thingSpawnTex);
}

void DrawAQuad()
{
	glClearColor(1.0, 1.0, 1.0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(-1., 1., -1., 1., 1., 20.);
	
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(0., 0., 10., 0., 0., 0., 0., 1., 0.);
	
	
	
	glBindTexture(GL_TEXTURE_2D, testtexid);
	
	glBegin(GL_QUADS);
		
		glTexCoord2f(0, 0); glVertex3f(-.75, -.75, 0.);
		glTexCoord2f(1, 0); glVertex3f( .75, -.75, 0.);
		glTexCoord2f(1, 1); glVertex3f( .75,  .75, 0.);
		glTexCoord2f(0, 1); glVertex3f(-.75,  .75, 0.);
	glEnd();
	
	/*glBegin(GL_QUADS);
		glColor3f(1., 0., 0.); glVertex3f(-.75, -.75, 0.);
		glColor3f(0., 1., 0.); glVertex3f( .75, -.75, 0.);
		glColor3f(0., 0., 1.); glVertex3f( .75,  .75, 0.);
		glColor3f(1., 1., 0.); glVertex3f(-.75,  .75, 0.);
	glEnd();*/
	
	printf("GL Error?  : %d\n", glGetError());
}

void CreateTestTexture()
{
	unsigned char c;
	unsigned char alpha = 0x80;
	
	for(c = 0; c < (8 * 8); c++)
	{
		testTex[c * 4] = (GLubyte)0x80;
		testTex[(c * 4) + 1] = (GLubyte)0x00;
		testTex[(c * 4) + 2] = (GLubyte)0x00;
		testTex[(c * 4) + 3] = (GLubyte)alpha;
	}
	
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	
	glGenTextures(1, &testtexid);
	glBindTexture(GL_TEXTURE_2D, testtexid);
	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, 
					GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, 
					GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 8, 
					8, 0, GL_RGBA, GL_UNSIGNED_BYTE, 
					testTex);
					
}

void GetCmdEditorCoords(int argc, char ** argv)
{
	unsigned short x;
	char instr[16];
	
	for(x = 0; x < 128; x++)
	{
		bzero(&instr, 16);
		sprintf(instr, "x%d", x);
		
		if(IsCmdOptionSet(argc, argv, instr)) { editorX = x; }
		
		bzero(&instr, 16);
		sprintf(instr, "y%d", x);
		
		if(IsCmdOptionSet(argc, argv, instr)) { editorY = x; }
	}
}

void Selecter_MoveLeft()
{
	if(editorXSelect == 0)
	{
		if(editorZoom == ZOOM_MAP) { editorXSelect = 1; }
		else if(editorZoom == ZOOM_QUADRANT) { editorXSelect = 3; }
		else if(editorZoom == ZOOM_SCREEN) { editorXSelect = 3; }
		else { editorXSelect = 1; }
	}
	else
	{
		editorXSelect--;
	}
}

void Selecter_MoveRight()
{
	editorXSelect++;
	
	if(editorZoom == ZOOM_MAP &&  editorXSelect > 1) { editorXSelect = 0; }
	else if(editorZoom == ZOOM_QUADRANT && editorXSelect > 3) { editorXSelect = 0; }
	else if(editorZoom == ZOOM_SCREEN && editorXSelect > 3) { editorXSelect = 0; }
	else if(editorZoom == ZOOM_BLOCK && editorXSelect > 1) { editorXSelect = 0; }
}

void Selecter_MoveDown()
{
	editorYSelect++;
	
	if(editorZoom == ZOOM_MAP && editorYSelect > 1) { editorYSelect = 0; }
	else if(editorZoom == ZOOM_QUADRANT && editorYSelect > 3) { editorYSelect = 0; }
	else if(editorZoom == ZOOM_SCREEN && editorYSelect > 3) { editorYSelect = 0; }
	else if(editorZoom == ZOOM_BLOCK && editorYSelect > 1) { editorYSelect = 0; }
}

void Selecter_MoveUp()
{
	if(editorYSelect == 0)
	{
		if(editorZoom == ZOOM_MAP) { editorYSelect = 1; }
		else if(editorZoom == ZOOM_QUADRANT) { editorYSelect = 3; }
		else if(editorZoom == ZOOM_SCREEN) { editorYSelect = 3; }
		else { editorYSelect = 1; }
	}
	else
	{
		editorYSelect--;
	}
}

void RenderEditor()
{
	//printf("RenderEditor: x,y %d, %d\n", editorX, editorY);
	if(editorZoom == ZOOM_MAP) { RenderMap(editorLevel, editorMode, editorXSelect, editorYSelect, 0x01); }
	else if(editorZoom == ZOOM_QUADRANT) { RenderQuadrant(editorLevel, editorMode, (editorY * 2) + editorX, editorXSelect, editorYSelect, 0x01); }
	else if(editorZoom == ZOOM_SCREEN) { RenderScreen(editorLevel, editorMode, (editorY * 8) + editorX, editorXSelect, editorYSelect, 0x01); }
	else { RenderBlock(editorLevel, editorMode, (editorY * 32) + editorX); }
	
	glXSwapBuffers(dpy, win);
}
