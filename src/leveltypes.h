#include <stdlib.h>
#include <X11/X.h>
#include <X11/Xlib.h>
#include <GL/gl.h>
#include <GL/glx.h>
#include <GL/glu.h>
//#include "main.h"

typedef int bool;
#define true 1
#define false 0

typedef unsigned char LevelType;
#define LevelType_Tank 0x00
#define LevelType_Overhead 0x01

typedef unsigned char BlockID;
typedef unsigned char SubBlockID;
typedef unsigned char UltraSubBlockID;
typedef unsigned char ChrID;
//	Levels consist of:
//		Pointers to ROM data
//			Palette
//			USB Attribute Table
//			USB Table
//			SB Table
//			Block Table
//			Map
//		Palette
//		A map
//		A list of blocks
//		A list of sub blocks
//		A list of ultra sub blocks
//		A list of screen attributes (scroll borders)

typedef struct
{
	unsigned short listpointer;
	unsigned short typelistpointer;
	unsigned short xlistpointer;
	unsigned short ylistpointer;
} ThingListPointers;

typedef struct
{
	unsigned short palette;
	unsigned short usbattribute;
	unsigned short usbtable;
	unsigned short sbtable;
	unsigned short blocktable;
	unsigned short map;
	unsigned short scrolltable;
	ThingListPointers thinglist;
} LevelDataPointers;

typedef struct
{
	unsigned char color0;
	unsigned char color1;
	unsigned char color2;
	unsigned char color3;
} LevelPalette;

typedef struct
{
	unsigned char subpalette;
	bool gateway;
	bool tunnel;
	bool damaging;
	bool softladder;
	bool water;
	bool solid;
	bool lava;
	bool solidladder;
	bool destroyable;
	bool doorway;
	bool waterladder;
	
	bool overhang;
	bool ice;
} USBAttributes;

typedef struct
{
	unsigned char thingtype;
	unsigned char x;
	unsigned char y;
} Thing;

typedef struct
{
	unsigned char x;
	unsigned char y;
} SpawnCoords;

typedef struct
{
	GLubyte data[1024];
	GLuint texid;
} USBTexture;

typedef struct
{
	LevelType leveltype;
	unsigned short levelid;
	unsigned short romlevelpointer;
	LevelDataPointers datapointers;
	BlockID Map[32][32];
	SubBlockID Blocks[256][4];
	UltraSubBlockID SubBlocks[256][4];
	ChrID UltraSubBlocks[256][4];
	USBAttributes USBAttributeTable[256];
	USBTexture USBTextures[256];
	LevelPalette SubPalettes[4];
	unsigned char ScrollTable[16];
	Thing Things[512];
	SpawnCoords SpawnPoint;
} Level;

Level Levels[8][2];
