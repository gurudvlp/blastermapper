#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <X11/X.h>
#include <X11/Xlib.h>
#include <GL/gl.h>
#include <GL/glx.h>
#include <GL/glu.h>

#include "main.h"
#include "loadrom.h"
#include "palette.h"
//#include "leveltypes.h"

bool LoadRom(char * romfile)
{
	char rom[strlen(romfile) + 1];
	strcpy(rom, romfile);
	printf("Opening %s\n", rom);
	
	RomFile = fopen(rom, "rb");
	if(RomFile)
	{
		bzero(&RomHeader, 16);
		//fgets((char *)&RomHeader, 16, (FILE *)RomFile);
		fread(RomHeader, 16, 1, RomFile);
		
		if(RomHeader[0] != 0x4E
		|| RomHeader[1] != 0x45
		|| RomHeader[2] != 0x53
		|| RomHeader[3] != 0x1A)
		{
			fclose(RomFile);
			printf("Invalid ROM file.\n");
			exit(0);
		}
		
		unsigned short prgsize = RomHeader[4];
		unsigned short chrsize = RomHeader[5];
		
		if(prgsize != 8)
		{
			fclose(RomFile);
			printf("This ROM does not have 8 PRGROM banks.\n");
			exit(0);
		}
		
		if(chrsize != 16)
		{
			fclose(RomFile);
			printf("This ROM does not have 16 CHRROM banks.\n");
			exit(0);
		}
		
		
		int ebank;
		for(ebank = 0; ebank < 8; ebank++)
		{
			//fgets((char *)&PrgRom[ebank], 0x4000, (FILE *)RomFile);
			fread(PrgRom[ebank], 0x4000, 1, RomFile);
		}
		
		for(ebank = 0; ebank < 16; ebank++)
		{
			//fgets((char *)&ChrRom[ebank][0], 0x2000, (FILE *)RomFile);
			fread(ChrRom[ebank], 0x2000, 1, RomFile);
		}
		
		fclose(RomFile);
		
		printf("ROM successfully loaded to RAM.\n");
		
		InitializeLevels();
		
		if(!FindLevelPointers())
		{
			printf("Could not determine the pointers to the level data.\n");
			exit(0);
		}
		
		int el;
		for(el = 0; el < 8; el++)
		{
			if(!LoadLevel((Level *)&Levels[el][0]))
			{
				printf("Failed to load level %d (tank)\n", el); 
				exit(0);
			}
			
			if(!LoadLevel((Level *)&Levels[el][1]))
			{
				printf("Failed to load level %d (overhead)\n", el);
				exit(0);
			}
		}
		//printf("Level 0 ptr %04x\n", Levels[0][0].romlevelpointer);
		//LoadLevel((Level *)&Levels[0][0]);
		
		printf("Levels successfully loaded.\n");
	}
	else
	{
		printf("File does not exist!\n");
		exit(0);
	}
	
	return true;
}

void InitializeLevels()
{
	int el;
	
	for(el = 0; el < 8; el++)
	{
		bzero(&Levels[el][0], sizeof(Level));
		bzero(&Levels[el][1], sizeof(Level));
		
		Levels[el][0].levelid = el;
		Levels[el][1].levelid = el;
		Levels[el][0].leveltype = LevelType_Tank;
		Levels[el][1].leveltype = LevelType_Overhead;
		
	}
}

bool FindLevelPointers()
{
	//	Levels 1-5 tank mode is in bank 0
	//	Levels 6-8 tank and 1 and 3 overhead is in bank 1
	//	Levels 2, 4-8 overhead is in bank 2
	//	Thing (enemy/item/sprite) and other unknown data in bank 3
	
	int el;
	unsigned short addr;
	

	for(el = 0; el < 5; el++)
	{
		
		
		addr = PrgRom[0][(el * 4) + 1] << 8;
		addr += PrgRom[0][(el * 4)];
		addr -= 0x8000;
		Levels[el][0].romlevelpointer = addr;
		//printf("Level %d start pointer: 0x%04x\n", el, addr);
		
		addr = PrgRom[0][(el * 4) + 3] << 8;
		addr += PrgRom[0][(el * 4) + 2];
		addr -= 0x8000;
		Levels[el][0].datapointers.scrolltable = addr;
	}
	
	for(el = 0; el < 3; el++)
	{
		
		
		addr = PrgRom[1][(el * 4) + 1] << 8;
		addr += PrgRom[1][(el * 4)];
		addr -= 0x8000;
		Levels[el + 5][0].romlevelpointer = addr;
		
		addr = PrgRom[1][(el * 4) + 3] << 8;
		addr += PrgRom[1][(el * 4) + 2];
		addr -= 0x8000;
		Levels[el + 5][0].datapointers.scrolltable = addr;
	}
	
	//	Now for overhead 1 & 3
	addr = PrgRom[1][13] << 8;
	addr += PrgRom[1][12];
	addr -= 0x8000;
	Levels[0][1].romlevelpointer = addr;
	
	addr = PrgRom[1][15] << 8;
	addr += PrgRom[1][14];
	addr -= 0x8000;
	Levels[0][1].datapointers.scrolltable = addr;
	
	addr = PrgRom[1][17] << 8;
	addr += PrgRom[1][16];
	addr -= 0x8000;
	Levels[2][1].romlevelpointer = addr;
	
	addr = PrgRom[1][19] << 8;
	addr += PrgRom[1][18];
	addr -= 0x8000;
	Levels[2][1].datapointers.scrolltable = addr;
	
	/*addr = PrgRom[1][12 + 1] << 8;
	addr += PrgRom[1][12];
	addr -= 0x8000;
	Levels[0][1].romlevelpointer = addr;
	
	addr = PrgRom[1][12 + 3] << 8;
	addr += PrgRom[1][12 + 2];
	addr -= 0x8000;
	Levels[0][1].datapointers.scrolltable = addr;
	
	addr = PrgRom[1][16 + 1] << 8;
	addr += PrgRom[1][16];
	addr -= 0x8000;
	Levels[2][1].romlevelpointer = addr;
	
	addr = PrgRom[1][16 + 3] << 8;
	addr += PrgRom[1][16 + 2];
	addr -= 0x8000;
	Levels[2][1].datapointers.scrolltable = addr;*/
	
	//	level 2 overhead
	addr = PrgRom[2][5] << 8;
	addr += PrgRom[2][4];
	addr -= 0x8000;
	Levels[1][1].romlevelpointer = addr;
	
	addr = PrgRom[2][7] << 8;
	addr += PrgRom[2][6];
	addr -= 0x8000;
	Levels[1][1].datapointers.scrolltable = addr;
	
	//	level 4 overhead
	addr = PrgRom[2][17] << 8;
	addr += PrgRom[2][16];
	addr -= 0x8000;
	Levels[3][1].romlevelpointer = addr;
	
	addr = PrgRom[2][19] << 8;
	addr += PrgRom[2][18];
	addr -= 0x8000;
	Levels[3][1].datapointers.scrolltable = addr;
	
	//	level 5 overhead
	addr = PrgRom[2][1] << 8;
	addr += PrgRom[2][0];
	addr -= 0x8000;
	Levels[4][1].romlevelpointer = addr;
	
	addr = PrgRom[2][3] << 8;
	addr += PrgRom[2][2];
	addr -= 0x8000;
	Levels[4][1].datapointers.scrolltable = addr;
	
	//	level 6 overhead
	addr = PrgRom[2][9] << 8;
	addr += PrgRom[2][8];
	addr -= 0x8000;
	Levels[5][1].romlevelpointer = addr;
	
	addr = PrgRom[2][11] << 8;
	addr += PrgRom[2][10];
	addr -= 0x8000;
	Levels[5][1].datapointers.scrolltable = addr;
	
	//	level 7 overhead
	addr = PrgRom[2][21] << 8;
	addr += PrgRom[2][20];
	addr -= 0x8000;
	Levels[6][1].romlevelpointer = addr;
	
	addr = PrgRom[2][23] << 8;
	addr += PrgRom[2][22];
	addr -= 0x8000;
	Levels[6][1].datapointers.scrolltable = addr;
	
	//	level 8 overhead
	addr = PrgRom[2][13] << 8;
	addr += PrgRom[2][12];
	addr -= 0x8000;
	Levels[7][1].romlevelpointer = addr;
	
	addr = PrgRom[2][15] << 8;
	addr += PrgRom[2][14];
	addr -= 0x8000;
	Levels[7][1].datapointers.scrolltable = addr;
	
	//	levels 4-8 overhead
	/*for(el = 1; el < 6; el++)
	{

		addr = PrgRom[2][(el * 4) + 1] << 8;
		addr += PrgRom[2][(el * 4)];
		addr -= 0x8000;
		Levels[el + 2][1].romlevelpointer = addr;
		
		addr = PrgRom[2][(el * 4) + 3] << 8;
		addr += PrgRom[2][(el * 4) + 2];
		addr -= 0x8000;
		Levels[el + 2][1].datapointers.scrolltable = addr;
	}*/
	
	//	And find thing pointers
	
	unsigned short thingptrptr;
	thingptrptr = PrgRom[3][3] << 8;
	thingptrptr += PrgRom[3][2];
	thingptrptr -= 0x8000;
	
	unsigned short typeptr;
	unsigned short xptr;
	unsigned short yptr;
	
	for(el = 0; el < 16; el++)
	{
		addr = PrgRom[3][thingptrptr + (el * 2) + 1] << 8;
		addr += PrgRom[3][thingptrptr + (el * 2)];
		addr -= 0x8000;
		
		typeptr = PrgRom[3][addr + 1] << 8;
		typeptr += PrgRom[3][addr];
		typeptr -= 0x8000;
		
		xptr = PrgRom[3][addr + 3] << 8;
		xptr += PrgRom[3][addr + 2];
		xptr -= 0x8000;
		
		yptr = PrgRom[3][addr + 5] << 8;
		yptr += PrgRom[3][addr + 4];
		yptr -= 0x8000;
		
		
		if(el < 8)
		{
			Levels[el][1].datapointers.thinglist.listpointer = addr;
			Levels[el][1].datapointers.thinglist.typelistpointer = typeptr;
			Levels[el][1].datapointers.thinglist.xlistpointer = xptr;
			Levels[el][1].datapointers.thinglist.ylistpointer = yptr;
		}
		else
		{
			Levels[el - 8][0].datapointers.thinglist.listpointer = addr;
			Levels[el - 8][0].datapointers.thinglist.typelistpointer = typeptr;
			Levels[el - 8][0].datapointers.thinglist.xlistpointer = xptr;
			Levels[el - 8][0].datapointers.thinglist.ylistpointer = yptr;
		}
	}
	
	//	?	hopefully :x
	return true;
}

bool LoadLevel(Level * level)
{
	//	Load data pointer:
	//		Palette
	//		USB Attribute tables
	//		USB table
	//		SB Table
	//		Block table
	//		Map
	
	int bank = 0;
	if((*level).leveltype == LevelType_Tank)
	{
		if((*level).levelid < 5) { bank = 0; }
		else { bank = 1; }
	}
	else
	{
		if((*level).levelid == 0 || ((*level).levelid == 2)) { bank = 1; }
		else { bank = 2; }
	}
	
	
	//	Load the data pointers
	//printf("Load level %d starting at 0x%04x\n", (*level).levelid, (*level).romlevelpointer);
	(*level).datapointers.palette = PrgRom[bank][(*level).romlevelpointer + 1] << 8;
	(*level).datapointers.palette += PrgRom[bank][(*level).romlevelpointer];
	(*level).datapointers.palette -= 0x8000;
	
	(*level).datapointers.usbattribute = PrgRom[bank][(*level).romlevelpointer + 3] << 8;
	(*level).datapointers.usbattribute += PrgRom[bank][(*level).romlevelpointer + 2];
	(*level).datapointers.usbattribute -= 0x8000;
	
	(*level).datapointers.usbtable = PrgRom[bank][(*level).romlevelpointer + 5] << 8;
	(*level).datapointers.usbtable += PrgRom[bank][(*level).romlevelpointer + 4];
	(*level).datapointers.usbtable -= 0x8000;
	
	(*level).datapointers.sbtable = PrgRom[bank][(*level).romlevelpointer + 7] << 8;
	(*level).datapointers.sbtable += PrgRom[bank][(*level).romlevelpointer + 6];
	(*level).datapointers.sbtable -= 0x8000;
	
	(*level).datapointers.blocktable = PrgRom[bank][(*level).romlevelpointer + 9] << 8;
	(*level).datapointers.blocktable += PrgRom[bank][(*level).romlevelpointer + 8];
	(*level).datapointers.blocktable -= 0x8000;
	
	(*level).datapointers.map = PrgRom[bank][(*level).romlevelpointer + 11] << 8;
	(*level).datapointers.map += PrgRom[bank][(*level).romlevelpointer + 10];
	(*level).datapointers.map -= 0x8000;
	
	//	Load the sub palettes
	int esp;
	for(esp = 0; esp < 4; esp++)
	{
		(*level).SubPalettes[esp].color0 = PrgRom[bank][(*level).datapointers.palette + (esp * 4)];
		(*level).SubPalettes[esp].color1 = PrgRom[bank][(*level).datapointers.palette + (esp * 4) + 1];
		(*level).SubPalettes[esp].color2 = PrgRom[bank][(*level).datapointers.palette + (esp * 4) + 2];
		(*level).SubPalettes[esp].color3 = PrgRom[bank][(*level).datapointers.palette + (esp * 4) + 3];
	}
	
	
	
	//	Now onto the map.
	//	I'm loading the map first because the highest block id needs to be found to know
	//	how many blocks to load.
	
	int x, y;
	unsigned short highblockid = 0;
	unsigned short basemapaddr = (*level).datapointers.map;
	
	//printf("Map address is: %04x / %04x\n", basemapaddr, basemapaddr + 0x8000);
	
	for(y = 0; y < 32; y++)
	{
		for(x = 0; x < 32; x++)
		{
			(*level).Map[x][y] = (BlockID)PrgRom[bank][basemapaddr + (y * 32) + x];
			if((*level).Map[x][y] > highblockid) { highblockid = (*level).Map[x][y]; }
		}
	}
	
	//printf("Level %d: Highest BlockID %d\n", (*level).levelid, highblockid);
	
	unsigned short highsbid = 0;
	unsigned short baseblockaddr = (*level).datapointers.blocktable;
	
	for(x = 0; x <= highblockid; x++)
	{
		(*level).Blocks[x][0] = (SubBlockID)PrgRom[bank][baseblockaddr + (x * 4)];
		if((*level).Blocks[x][0] > highsbid) { highsbid = (*level).Blocks[x][0]; }
		
		(*level).Blocks[x][1] = (SubBlockID)PrgRom[bank][baseblockaddr + (x * 4) + 1];
		if((*level).Blocks[x][1] > highsbid) { highsbid = (*level).Blocks[x][2]; }
		
		(*level).Blocks[x][2] = (SubBlockID)PrgRom[bank][baseblockaddr + (x * 4) + 2];
		if((*level).Blocks[x][2] > highsbid) { highsbid = (*level).Blocks[x][2]; }
		
		(*level).Blocks[x][3] = (SubBlockID)PrgRom[bank][baseblockaddr + (x * 4) + 3];
		if((*level).Blocks[x][3] > highsbid) { highsbid = (*level).Blocks[x][3]; }
	}
	
	//printf("Level %d: Highest SubBlockID %d\n", (*level).levelid, highsbid);
	
	unsigned short highusbid = 0;
	unsigned short basesbaddr = (*level).datapointers.sbtable;
	
	for(x = 0; x <= highsbid; x++)
	{
		(*level).SubBlocks[x][0] = (UltraSubBlockID)PrgRom[bank][basesbaddr + (x * 4)];
		if((*level).SubBlocks[x][0] > highusbid) { highusbid = (*level).SubBlocks[x][0]; }
		
		(*level).SubBlocks[x][1] = (UltraSubBlockID)PrgRom[bank][basesbaddr + (x * 4) + 1];
		if((*level).SubBlocks[x][1] > highusbid) { highusbid = (*level).SubBlocks[x][1]; }
		
		(*level).SubBlocks[x][2] = (UltraSubBlockID)PrgRom[bank][basesbaddr + (x * 4) + 2];
		if((*level).SubBlocks[x][2] > highusbid) { highusbid = (*level).SubBlocks[x][2]; }
		
		(*level).SubBlocks[x][3] = (UltraSubBlockID)PrgRom[bank][basesbaddr + (x * 4) + 3];
		if((*level).SubBlocks[x][3] > highusbid) { highusbid = (*level).SubBlocks[x][3]; }
	}
	
	//printf("Level %d: Highest UltraSubBlockID %d\n", (*level).levelid, highusbid);
	
	unsigned short baseusbaddr = (*level).datapointers.usbtable;
	
	for(x = 0; x <= highusbid; x++)
	{
		(*level).UltraSubBlocks[x][0] = (ChrID)PrgRom[bank][baseusbaddr + (x * 4)];
		(*level).UltraSubBlocks[x][1] = (ChrID)PrgRom[bank][baseusbaddr + (x * 4) + 1];
		(*level).UltraSubBlocks[x][2] = (ChrID)PrgRom[bank][baseusbaddr + (x * 4) + 2];
		(*level).UltraSubBlocks[x][3] = (ChrID)PrgRom[bank][baseusbaddr + (x * 4) + 3];
	}
	
	//	And to the USB attribute table
	unsigned short baseusba = (*level).datapointers.usbattribute;
	
	for(x = 0; x <= highusbid; x++)
	{
		unsigned char tbyte = PrgRom[bank][baseusba + x];
		
		if((*level).leveltype == LevelType_Overhead)
		{
			if((tbyte & 0x14) == 0x14) { (*level).USBAttributeTable[x].ice = true; }
			else { (*level).USBAttributeTable[x].ice = false; }
			
			if((tbyte & 0x88) == 0x88) { (*level).USBAttributeTable[x].overhang = true; }
			else { (*level).USBAttributeTable[x].overhang = false; }
		}
		
		if((tbyte & 0x08) == 0x08) { (*level).USBAttributeTable[x].gateway = true; }
		if((tbyte & 0x0C) == 0x0C) { (*level).USBAttributeTable[x].tunnel = true; }
		if((tbyte & 0x10) == 0x10) { (*level).USBAttributeTable[x].damaging = true; }
		if((tbyte & 0x20) == 0x20) { (*level).USBAttributeTable[x].softladder = true; }
		if((tbyte & 0x40) == 0x40) { (*level).USBAttributeTable[x].water = true; }
		if((tbyte & 0x80) == 0x80) { (*level).USBAttributeTable[x].solid = true; }
		if((tbyte & 0x50) == 0x50) { (*level).USBAttributeTable[x].lava = true; }
		if((tbyte & 0xA0) == 0xA0) { (*level).USBAttributeTable[x].solidladder = true; }
		if((tbyte & 0xC0) == 0xC0) { (*level).USBAttributeTable[x].destroyable = true; }
		if((tbyte & 0x88) == 0x88) { (*level).USBAttributeTable[x].doorway = true; }
		if((tbyte & 0x60) == 0x60) { (*level).USBAttributeTable[x].waterladder = true; }
		
		(*level).USBAttributeTable[x].subpalette = 0x00;
		if((tbyte & 0x01) == 0x01) { (*level).USBAttributeTable[x].subpalette++; }
		if((tbyte & 0x02) == 0x02) { (*level).USBAttributeTable[x].subpalette += 2; }
	}
	
	//printf("Level %d: Ultra Sub Block Attributes loaded\n", (*level).levelid);
	
	//	And the scroll table
	unsigned short basestaddr = (*level).datapointers.scrolltable;
	
	for(x = 0; x < 16; x++)
	{
		(*level).ScrollTable[x] = PrgRom[bank][basestaddr + x];
	}
	
	//printf("Level %d: Scroll table loaded (but not understood yet)\n", (*level).levelid);
	
	//	Load the list of things
	
	unsigned short thingcount = 0;
	while(1)
	{
		unsigned char tthing = PrgRom[3][(*level).datapointers.thinglist.typelistpointer + thingcount];
		if(tthing == 0xFF) { break; }
		if(thingcount >= 512) { break; }
		
		unsigned char thingx = PrgRom[3][(*level).datapointers.thinglist.xlistpointer + thingcount];
		unsigned char thingy = PrgRom[3][(*level).datapointers.thinglist.ylistpointer + thingcount];
		
		(*level).Things[thingcount].thingtype = tthing;
		(*level).Things[thingcount].x = thingx;
		(*level).Things[thingcount].y = thingy;
		thingcount++;
	}
	
	if(thingcount < 512) { (*level).Things[thingcount + 1].thingtype = 0xFF; }
	
	
	//	Load spawn point
	FindSpawnPoint(level);
	//printf("Level %d: Spawn (%02x, %02x)\n", level->levelid, level->SpawnPoint.x, level->SpawnPoint.y);
	
	return true;
}









void LoadUSBTextures(int level, int lvlmode)
{
	CreateUSBTextures((Level *)&Levels[level][lvlmode]);
}

void CreateUSBTextures(Level * level)
{
	//printf("Level %d: Creating textures\n", (*level).levelid);
	unsigned short offset = 0x00;
	int bank = 0;
	
	if((*level).leveltype == LevelType_Tank)
	{
		if((*level).levelid == 0) { bank = 4; }
		if((*level).levelid == 1) { bank = 4; offset = 0x1000; }
		if((*level).levelid == 2) { bank = 5; }
		if((*level).levelid == 3) { bank = 5; offset = 0x1000; }
		if((*level).levelid == 4) { bank = 6; }
		if((*level).levelid == 5) { bank = 6; offset = 0x1000; }
		if((*level).levelid == 6) { bank = 7; }
		if((*level).levelid == 7) { bank = 7; offset = 0x1000; }
	}
	else
	{
		if(((*level).levelid == 0) || ((*level).levelid == 7)) { bank = 8; offset = 0x1000; }
		if(((*level).levelid == 2) || ((*level).levelid == 4)) { bank = 9; }
		if(((*level).levelid == 1) || ((*level).levelid == 5)) { bank = 9; offset = 0x1000; }
		if(((*level).levelid == 3) || ((*level).levelid == 6)) { bank = 10; }
	}
	
	int x, y;
	unsigned char tiledata[256][8][8];
	
	for(y = 0; y < 16; y++)
	{
		for(x = 0; x < 16; x++)
		{
			int pty;
			for(pty = 0; pty < 8; pty++)
			{
				unsigned char chbyte = ChrRom[bank][offset];
				unsigned char plbyte = ChrRom[bank][offset + 8];
				
				tiledata[(y * 16) + x][0][pty] = ((chbyte & 128) >> 7) + (((plbyte & 128) >> 7) * 2);
				tiledata[(y * 16) + x][1][pty] = ((chbyte & 64) >> 6) + (((plbyte & 64) >> 6) * 2);
				tiledata[(y * 16) + x][2][pty] = ((chbyte & 32) >> 5) + (((plbyte & 32) >> 5) * 2);
				tiledata[(y * 16) + x][3][pty] = ((chbyte & 16) >> 4) + (((plbyte & 16) >> 4) * 2);
				tiledata[(y * 16) + x][4][pty] = ((chbyte & 8) >> 3) + (((plbyte & 8) >> 3) * 2);
				tiledata[(y * 16) + x][5][pty] = ((chbyte & 4) >> 2) + (((plbyte & 4) >> 2) * 2);
				tiledata[(y * 16) + x][6][pty] = ((chbyte & 2) >> 1) + (((plbyte & 2) >> 1) * 2);
				tiledata[(y * 16) + x][7][pty] = (chbyte & 1) + ((plbyte & 1) * 2);
				
				offset++;
			}
			
			offset += 8;
		}
	}
	
	//	All of the bytes of the chr data have been parsed, so now
	//	we need to loop through the ultra sub blocks, and assemble each
	//	one
	
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	
	int eusb;
	for(eusb = 0; eusb < 256; eusb++)
	{
		//printf("\tUSB %d\n", eusb);
		glGenTextures(1, &(*level).USBTextures[eusb].texid);
		glBindTexture(GL_TEXTURE_2D, (*level).USBTextures[eusb].texid);
		
		unsigned char upal[4];
		unsigned char subpal = (*level).USBAttributeTable[eusb].subpalette;
		upal[0] = (*level).SubPalettes[subpal].color0;
		upal[1] = (*level).SubPalettes[subpal].color1;
		upal[2] = (*level).SubPalettes[subpal].color2;
		upal[3] = (*level).SubPalettes[subpal].color3;
		
		//	Upper left chr
		for(y = 0; y < 8; y++)
		{
			for(x = 0; x < 8; x++)
			{
				//printf("\t\tUSB x,y: %d, %d\n", x, y);
				unsigned char chr = (*level).UltraSubBlocks[eusb][0];
				unsigned char chrcol = tiledata[chr][x][y];
				unsigned char pixcol = upal[chrcol];
				
				unsigned char pixbytes[4];
				pixbytes[0] = Palette[pixcol][0];
				pixbytes[1] = Palette[pixcol][1];
				pixbytes[2] = Palette[pixcol][2];
				pixbytes[3] = 0xFF;
				
				(*level).USBTextures[eusb].data[(y * 64) + (x * 4)] = pixbytes[0];
				(*level).USBTextures[eusb].data[(y * 64) + (x * 4) + 1] = pixbytes[1];
				(*level).USBTextures[eusb].data[(y * 64) + (x * 4) + 2] = pixbytes[2];
				(*level).USBTextures[eusb].data[(y * 64) + (x * 4) + 3] = pixbytes[3];
			}
		}
		
		//	Upper right chr
		for(y = 0; y < 8; y++)
		{
			for(x = 0; x < 8; x++)
			{
				//printf("\t\tUSB x,y: %d, %d\n", x, y);
				unsigned char chr = (*level).UltraSubBlocks[eusb][1];
				unsigned char chrcol = tiledata[chr][x][y];
				unsigned char pixcol = upal[chrcol];
				
				unsigned char pixbytes[4];
				pixbytes[0] = Palette[pixcol][0];
				pixbytes[1] = Palette[pixcol][1];
				pixbytes[2] = Palette[pixcol][2];
				pixbytes[3] = 0xFF;
				
				(*level).USBTextures[eusb].data[(y * 64) + (x * 4) + 32] = pixbytes[0];
				(*level).USBTextures[eusb].data[(y * 64) + (x * 4) + 33] = pixbytes[1];
				(*level).USBTextures[eusb].data[(y * 64) + (x * 4) + 34] = pixbytes[2];
				(*level).USBTextures[eusb].data[(y * 64) + (x * 4) + 35] = pixbytes[3];
			}
		}
		
		
		//	Lower left chr
		for(y = 0; y < 8; y++)
		{
			for(x = 0; x < 8; x++)
			{
				//printf("\t\tUSB x,y: %d, %d\n", x, y);
				unsigned char chr = (*level).UltraSubBlocks[eusb][2];
				unsigned char chrcol = tiledata[chr][x][y];
				unsigned char pixcol = upal[chrcol];
				
				unsigned char pixbytes[4];
				pixbytes[0] = Palette[pixcol][0];
				pixbytes[1] = Palette[pixcol][1];
				pixbytes[2] = Palette[pixcol][2];
				pixbytes[3] = 0xFF;
				
				(*level).USBTextures[eusb].data[(y * 64) + (x * 4) + 512] = pixbytes[0];
				(*level).USBTextures[eusb].data[(y * 64) + (x * 4) + 513] = pixbytes[1];
				(*level).USBTextures[eusb].data[(y * 64) + (x * 4) + 514] = pixbytes[2];
				(*level).USBTextures[eusb].data[(y * 64) + (x * 4) + 515] = pixbytes[3];
			}
		}
		
		//	Lower right chr
		for(y = 0; y < 8; y++)
		{
			for(x = 0; x < 8; x++)
			{
				//printf("\t\tUSB x,y: %d, %d\n", x, y);
				unsigned char chr = (*level).UltraSubBlocks[eusb][3];
				unsigned char chrcol = tiledata[chr][x][y];
				unsigned char pixcol = upal[chrcol];
				
				unsigned char pixbytes[4];
				pixbytes[0] = Palette[pixcol][0];
				pixbytes[1] = Palette[pixcol][1];
				pixbytes[2] = Palette[pixcol][2];
				pixbytes[3] = 0xFF;
				
				(*level).USBTextures[eusb].data[(y * 64) + (x * 4) + 544] = pixbytes[0];
				(*level).USBTextures[eusb].data[(y * 64) + (x * 4) + 545] = pixbytes[1];
				(*level).USBTextures[eusb].data[(y * 64) + (x * 4) + 546] = pixbytes[2];
				(*level).USBTextures[eusb].data[(y * 64) + (x * 4) + 547] = pixbytes[3];
			}
		}
		
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, 
						GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, 
						GL_LINEAR);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 16, 
						16, 0, GL_RGBA, GL_UNSIGNED_BYTE, 
						(*level).USBTextures[eusb].data);
	}
}

void FindSpawnPoint(Level * level)
{
	//	overhead starts at 0x1c5B2 in rom
	//	tank is 0x1ca4b
	//	both minus 0x10 (16 bytes) for rom header
	unsigned short addr = 0x0000;
	unsigned short bank = 7;
	
	if(level->leveltype == LevelType_Overhead) { addr = 0x05A2; }
	else { addr = 0x0A3B; }
	
	addr += level->levelid * 2;
	
	level->SpawnPoint.x = PrgRom[bank][addr];
	level->SpawnPoint.y = PrgRom[bank][addr + 1];
}

void PrintThings(int level, unsigned char leveltype)
{
	if(level < 0 || level > 7) { return; }
	if(leveltype != 0x00 && leveltype != 0x01) { return; }
	
	Level * lvl = (Level *)&Levels[level][leveltype];
	
	printf("Thing list for level %d (mode %x)\n", level, leveltype);
	
	int x;
	for(x = 0; x < 512; x++)
	{
		if((*lvl).Things[x].thingtype == 0xFF) { break; }
		
		printf("Thing %d: Type: %02x\t(%02x, %02x)\n",
			x,
			(*lvl).Things[x].thingtype,
			(*lvl).Things[x].x,
			(*lvl).Things[x].y
		);
	}
}

void PrintLevelPointer(char * text, unsigned char * bytes)
{
	if(SAVEROM_SHOW_LEVEL_POINTERS)
	{
		printf("%s: %02x %02x\n", text, bytes[0], bytes[1]);
	}
}
