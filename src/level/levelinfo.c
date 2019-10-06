#include <stdlib.h>
#include <stdio.h>

#include "leveltypes.h"
#include "levelinfo.h"

unsigned char GetHighestBlockID(int lvl, int lvlmode)
{
	if(lvl > 7 || lvl < 0) { return 0x00; }
	if(lvlmode < 0 || lvlmode > 1) { return 0x00; }
	Level * level = (Level *)&Levels[lvl][lvlmode];
	
	int x, y;
	unsigned char highest = 0x00;
	
	for(y = 0; y < 32; y++)
	{
		for(x = 0; x < 32; x++)
		{
			if(level->Map[x][y] > highest) { highest = level->Map[x][y]; }
		}
	}
	
	return highest;
}

unsigned char GetHighestSubBlockID(int lvl, int lvlmode)
{
	if(lvl > 7 || lvl < 0) { return 0x00; }
	if(lvlmode < 0 || lvlmode > 1) { return 0x00; }
	Level * level = (Level *)&Levels[lvl][lvlmode];
	
	unsigned char highblock = GetHighestBlockID(lvl, lvlmode);
	
	int eb, esb;
	unsigned char highest = 0x00;
	
	for(eb = 0; eb <= highblock; eb++)
	{
		for(esb = 0; esb < 4; esb++)
		{
			if(level->Blocks[eb][esb] > highest) { highest = level->Blocks[eb][esb]; }
		}
	}
	
	return highest;
}

unsigned char GetHighestUltraSubBlockID(int lvl, int lvlmode)
{
	if(lvl > 7 || lvl < 0) { return 0x00; }
	if(lvlmode < 0 || lvlmode > 1) { return 0x00; }
	Level * level = (Level *)&Levels[lvl][lvlmode];
	
	unsigned char highsb = GetHighestSubBlockID(lvl, lvlmode);
	
	int esb, eusb;
	unsigned char highest = 0x00;
	
	for(esb = 0; esb <= highsb; esb++)
	{
		for(eusb = 0; eusb < 4; eusb++)
		{
			if(level->SubBlocks[esb][eusb] > highest) { highest = level->SubBlocks[esb][eusb]; }
		}
	}
	
	return highest;
}

unsigned char BlockAt(int lvl, int lvlmode, int x, int y)
{
	if(lvl > 7 || lvl < 0) { return 0x00; }
	if(lvlmode < 0 || lvlmode > 1) { return 0x00; }
	Level * level = (Level *)&Levels[lvl][lvlmode];
	
	return (unsigned char)level->Map[x][y];
}

void SetBlockAt(int lvl, int lvlmode, int x, int y, unsigned char blockid)
{
	if(lvl > 7 || lvl < 0) { return; }
	if(lvlmode < 0 || lvlmode > 1) { return; }
	Level * level = (Level *)&Levels[lvl][lvlmode];
	
	level->Map[x][y] = (BlockID)blockid;
}

void SetSpawnPoint(int lvl, int lvlmode, int x, int y)
{
	if(lvl > 7 || lvl < 0) { return; }
	if(lvlmode < 0 || lvlmode > 1) { return; }
	Level * level = (Level *)&Levels[lvl][lvlmode];
	
	level->SpawnPoint.x = x & 0xFF;
	level->SpawnPoint.y = y & 0xFF;
}

void PrintSpawnPoints()
{
	int el;
	for(el = 0; el < 8; el++)
	{
		printf("Level %d Tank Spawn (%02x, %02x) (%d, %d)\n",
			el,
			Levels[el][0].SpawnPoint.x,
			Levels[el][0].SpawnPoint.y,
			Levels[el][0].SpawnPoint.x,
			Levels[el][0].SpawnPoint.y);
		
		printf("Level %d Overhead Spawn (%02x, %02x) (%d, %d)\n",
			el,
			Levels[el][1].SpawnPoint.x,
			Levels[el][1].SpawnPoint.y,
			Levels[el][1].SpawnPoint.x,
			Levels[el][1].SpawnPoint.y);
	}
}

//	GetThingAt is looking for the x and y coordinates of a sub block
short GetThingAt(int lvl, int lvlmode, int x, int y)
{
	if(lvl < 0 || lvl > 7) { return -1; }
	if(lvlmode < 0 || lvlmode > 1) { return -1; }
	Level * level = (Level *)&Levels[lvl][lvlmode];
	
	//printf("GetThingAt: Level: %d:%d, Coords: %d, %d\n", lvl, lvlmode, x, y);
	
	int et;
	for(et = 0; et < 512; et++)
	{
		if(level->Things[et].x == (x & 0xFF)
		&& level->Things[et].y == (y & 0xFF))
		{
			return et;
		}
	}
	
	return -1;
}

//	level_MapToBytes will take the map stored in the Level struct and convert
//	it to the binary format that the game can actually use.
//
//	unsigned char * mapdata
//		Where to store the binary map data.  This must be at least 1024 bytes.
//
//	Level * level
//		Pointer to the level that contains the map that should be converted to
//		binary.  This can be an overhead or side-view level.
//
//	Returns: The highest block id found on this map.
unsigned char level_MapToBytes(unsigned char * mapdata, Level * level)
{
	unsigned char highid = 0x00;
	int x, y;
	
	for(y = 0; y < 32; y++)
	{
		for(x = 0; x < 32; x++)
		{
			mapdata[x + (y * 32)] = (*level).Map[x][y];
			if((*level).Map[x][y] > highid) { highid = (*level).Map[x][y]; }
		}
	}
	
	return highid;
}


unsigned char level_USBAttributeToByte(USBAttributes * usbattr)
{
	unsigned char tbyte = 0x00;
	
	if((*usbattr).gateway) { tbyte |= 0x08; }
	if((*usbattr).tunnel) { tbyte |= 0x0C; }
	if((*usbattr).damaging) { tbyte |= 0x10; }
	if((*usbattr).softladder) { tbyte |= 0x20; }
	if((*usbattr).water) { tbyte |= 0x40; }
	if((*usbattr).solid) { tbyte |= 0x80; }
	if((*usbattr).lava) { tbyte |= 0x50; }
	if((*usbattr).solidladder) { tbyte |= 0xA0; }
	if((*usbattr).destroyable) { tbyte |= 0xC0; }
	if((*usbattr).doorway) { tbyte |= 0x88; }
	if((*usbattr).waterladder) { tbyte |= 0x60; }
	if((*usbattr).overhang) { tbyte |= 0x04; }
	if((*usbattr).ice) { tbyte |= 0x14; }
	
	tbyte |= (*usbattr).subpalette;
	
	return tbyte;
}
