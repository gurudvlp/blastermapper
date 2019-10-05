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
	
	printf("GetThingAt: Level: %d:%d, Coords: %d, %d\n", lvl, lvlmode, x, y);
	
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
