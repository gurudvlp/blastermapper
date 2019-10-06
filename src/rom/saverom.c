#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "loadrom.h"
#include "saverom.h"
#include "rombuilder.h"

bool SaveRom()
{
	//	Here goes nothing..
	//	Save the rebuilt ROM
	//
	//	See docs/Rom Layout.txt for an explanation of how the ROM is laid out.
	
	int eb;
	unsigned short echr;
	
	unsigned char  * OutRom = build_InitializeRom();
	
	build_CopyProgramming(OutRom);
	build_SpawnPoints(OutRom);
	build_Palettes(OutRom);
	build_ScrollTables(OutRom);
	build_LevelDataPointers(OutRom);
	
	//	Start with each set of data pointers for level data
	//	overhead levels 2, 5-8 are in bank 2,
	//	ordered like 5, 2, 6, 8, 4, 7


	
	////////////////////////////////////////////////////////////////////////////
	//
	//	Now to build the scroll tables for each level
	//
	
	//	Levels 1-5 tank
	for(echr = 0; echr < 5; echr++)
	{
		for(eb = 0; eb < 16; eb++)
		{
			OutRom[OFFSET_SCROLLTABLE + (16 * echr) + eb] = Levels[echr][0].ScrollTable[eb];
		}
	}
	
	//	Levels 6-8 tank
	for(echr = 0; echr < 3; echr++)
	{
		for(eb = 0; eb < 16; eb++)
		{
			OutRom[OFFSET_BANK_1 + OFFSET_SCROLLTABLE + (16 * echr) + eb] = Levels[echr + 5][0].ScrollTable[eb];
		}
	}
	
	//	Levels 1 & 3 overhead
	for(eb = 0; eb < 16; eb++)
	{
		OutRom[OFFSET_BANK_1 + OFFSET_SCROLLTABLE + 48 + eb] = Levels[0][1].ScrollTable[eb];
		OutRom[OFFSET_BANK_1 + OFFSET_SCROLLTABLE + 64 + eb] = Levels[2][1].ScrollTable[eb];
	}
	
	//	Levels 2, 5-8 overhead
	for(eb = 0; eb < 16; eb++)
	{	
		OutRom[OFFSET_BANK_2 + OFFSET_SCROLLTABLE + eb] = Levels[4][1].ScrollTable[eb];
		OutRom[OFFSET_BANK_2 + OFFSET_SCROLLTABLE + eb + 16] = Levels[1][1].ScrollTable[eb];
		OutRom[OFFSET_BANK_2 + OFFSET_SCROLLTABLE + eb + 32] = Levels[5][1].ScrollTable[eb];
		OutRom[OFFSET_BANK_2 + OFFSET_SCROLLTABLE + eb + 48] = Levels[7][1].ScrollTable[eb];
		OutRom[OFFSET_BANK_2 + OFFSET_SCROLLTABLE + eb + 64] = Levels[3][1].ScrollTable[eb];
		OutRom[OFFSET_BANK_2 + OFFSET_SCROLLTABLE + eb + 80] = Levels[6][1].ScrollTable[eb];
	}
	
	
	
	////////////////////////////////////////////////////////////////////////////
	//
	//	Map Data
	//
	
	//	Onto the map.  We need the actual map data, and also the highest block id
	unsigned char highblock[16];
	
	unsigned short datasize[16];
	
	highblock[0] = MapToBytes(&OutRom[OFFSET_MAP], &Levels[0][0]);
	highblock[1] = MapToBytes(&OutRom[OFFSET_MAP + 0x0400], &Levels[1][0]);
	highblock[2] = MapToBytes(&OutRom[OFFSET_MAP + 0x0800], &Levels[2][0]);
	highblock[3] = MapToBytes(&OutRom[OFFSET_MAP + 0x0C00], &Levels[3][0]);
	highblock[4] = MapToBytes(&OutRom[OFFSET_MAP + 0x1000], &Levels[4][0]);
	
	highblock[5] = MapToBytes(&OutRom[OFFSET_MAP + 0x4000], &Levels[5][0]);
	highblock[6] = MapToBytes(&OutRom[OFFSET_MAP + 0x4400], &Levels[6][0]);
	highblock[7] = MapToBytes(&OutRom[OFFSET_MAP + 0x4800], &Levels[7][0]);
	highblock[8] = MapToBytes(&OutRom[OFFSET_MAP + 0x4C00], &Levels[0][1]);
	highblock[9] = MapToBytes(&OutRom[OFFSET_MAP + 0x5000], &Levels[2][1]);
	
	highblock[10] = MapToBytes(&OutRom[OFFSET_MAP + 0x8000], &Levels[4][1]);
	highblock[11] = MapToBytes(&OutRom[OFFSET_MAP + 0x8400], &Levels[1][1]);
	highblock[12] = MapToBytes(&OutRom[OFFSET_MAP + 0x8800], &Levels[5][1]);
	highblock[13] = MapToBytes(&OutRom[OFFSET_MAP + 0x8C00], &Levels[7][1]);
	highblock[14] = MapToBytes(&OutRom[OFFSET_MAP + 0x9000], &Levels[3][1]);
	highblock[15] = MapToBytes(&OutRom[OFFSET_MAP + 0x9400], &Levels[6][1]);

	
	//	Build map data for each level.  This can be variable sizes, so that
	//	will need to be calculated too.
	//	Max size:
	//		256 blocks * 4 bytes:		1024b
	//		256 subblocks * 4 bytes:	1024b
	//		256 usbs * 4 bytes:			1024b
	//		256 usb attrs * 1 byte:		 256b
	//		Total:						3328b
	
	/*unsigned char mapmeta[3328];
	unsigned short blocksize = 0;
	unsigned short subblocksize = 0;
	unsigned short usbsize = 0;
	unsigned short usbattrsize = 0;*/
	
	
	//	Levels 1-5 tank
	//unsigned short nextstart = 0x1640;
	unsigned short ttlsize = 0;
	SerializedMapInfo smi[16];
	
	//	Areas 1-5 Tank
	datasize[0] = BuildMapData(&OutRom[OFFSET_MAPDATA], &Levels[0][0], highblock[0], &smi[0]);
	BuildMapPointers(&Levels[0][0], OFFSET_MAPDATA - SIZE_ROM_HEADER, &smi[0], &OutRom[0x30]);
	ttlsize += datasize[0];
	
	datasize[1] = BuildMapData(&OutRom[OFFSET_MAPDATA + ttlsize], &Levels[1][0], highblock[1], &smi[1]);
	BuildMapPointers(&Levels[1][0], OFFSET_MAPDATA - SIZE_ROM_HEADER + ttlsize, &smi[1], &OutRom[0x30 + 12]);
	ttlsize += datasize[1];
	
	datasize[2] = BuildMapData(&OutRom[OFFSET_MAPDATA + ttlsize], &Levels[2][0], highblock[2], &smi[2]);
	BuildMapPointers(&Levels[2][0], OFFSET_MAPDATA - SIZE_ROM_HEADER + ttlsize, &smi[2], &OutRom[0x30 + 24]);
	ttlsize += datasize[2];
	
	datasize[3] = BuildMapData(&OutRom[OFFSET_MAPDATA + ttlsize], &Levels[3][0], highblock[3], &smi[3]);
	BuildMapPointers(&Levels[3][0], OFFSET_MAPDATA - SIZE_ROM_HEADER + ttlsize, &smi[3], &OutRom[0x30 + 36]);
	ttlsize += datasize[3];
	
	datasize[4] = BuildMapData(&OutRom[OFFSET_MAPDATA + ttlsize], &Levels[4][0], highblock[4], &smi[4]);
	BuildMapPointers(&Levels[4][0], OFFSET_MAPDATA - SIZE_ROM_HEADER + ttlsize, &smi[4], &OutRom[0x30 + 48]);
	ttlsize += datasize[4];
	
	//	Levels 6-8 tank
	ttlsize = 0;
	datasize[5] = BuildMapData(&OutRom[OFFSET_MAPDATA + 0x4000], &Levels[5][0], highblock[5], &smi[5]);
	BuildMapPointers(&Levels[5][0], OFFSET_MAPDATA - SIZE_ROM_HEADER + ttlsize, &smi[5], &OutRom[0x4030]);
	ttlsize += datasize[5];
	
	datasize[6] = BuildMapData(&OutRom[OFFSET_MAPDATA + 0x4000 + ttlsize], &Levels[6][0], highblock[6], &smi[6]);
	BuildMapPointers(&Levels[6][0], OFFSET_MAPDATA - SIZE_ROM_HEADER + ttlsize, &smi[6], &OutRom[0x4030 + 12]);
	ttlsize += datasize[6];
	
	datasize[7] = BuildMapData(&OutRom[OFFSET_MAPDATA + 0x4000 + ttlsize], &Levels[7][0], highblock[7], &smi[7]);
	BuildMapPointers(&Levels[7][0], OFFSET_MAPDATA - SIZE_ROM_HEADER + ttlsize, &smi[7], &OutRom[0x4030 + 24]);
	ttlsize += datasize[7];
	
	//	Levels 1, 3 overhead
	datasize[8] = BuildMapData(&OutRom[OFFSET_MAPDATA + 0x4000 + ttlsize], &Levels[0][1], highblock[8], &smi[8]);
	BuildMapPointers(&Levels[0][1], OFFSET_MAPDATA - SIZE_ROM_HEADER + ttlsize, &smi[8], &OutRom[0x4030 + 36]);
	ttlsize += datasize[8];
	
	datasize[9] = BuildMapData(&OutRom[OFFSET_MAPDATA + 0x4000 + ttlsize], &Levels[2][1], highblock[9], &smi[9]);
	BuildMapPointers(&Levels[2][1], OFFSET_MAPDATA - SIZE_ROM_HEADER + ttlsize, &smi[9], &OutRom[0x4030 + 48]);
	ttlsize += datasize[9];
	
	
	//	Levels 2, 4-8 overhead
	ttlsize = 0;
	datasize[10] = BuildMapData(&OutRom[OFFSET_MAPDATA + 0x8000], &Levels[4][1], highblock[10], &smi[10]);
	BuildMapPointers(&Levels[4][1], OFFSET_MAPDATA - SIZE_ROM_HEADER + ttlsize, &smi[10], &OutRom[0x8030]);
	ttlsize += datasize[10];
	
	datasize[11] = BuildMapData(&OutRom[OFFSET_MAPDATA + 0x8000 + ttlsize], &Levels[1][1], highblock[11], &smi[11]);
	BuildMapPointers(&Levels[1][1], OFFSET_MAPDATA - SIZE_ROM_HEADER + ttlsize, &smi[11], &OutRom[0x8030 + 12]);
	ttlsize += datasize[11];
	
	datasize[12] = BuildMapData(&OutRom[OFFSET_MAPDATA + 0x8000 + ttlsize], &Levels[5][1], highblock[12], &smi[12]);
	BuildMapPointers(&Levels[5][1], OFFSET_MAPDATA - SIZE_ROM_HEADER + ttlsize, &smi[12], &OutRom[0x8030 + 24]);
	ttlsize += datasize[12];
	
	datasize[13] = BuildMapData(&OutRom[OFFSET_MAPDATA + 0x8000 + ttlsize], &Levels[7][1], highblock[13], &smi[13]);
	BuildMapPointers(&Levels[7][1], OFFSET_MAPDATA - SIZE_ROM_HEADER + ttlsize, &smi[13], &OutRom[0x8030 + 36]);
	ttlsize += datasize[13];
	
	datasize[14] = BuildMapData(&OutRom[OFFSET_MAPDATA + 0x8000 + ttlsize], &Levels[3][1], highblock[14], &smi[14]);
	BuildMapPointers(&Levels[3][1], OFFSET_MAPDATA - SIZE_ROM_HEADER + ttlsize, &smi[14], &OutRom[0x8030 + 48]);
	ttlsize += datasize[14];
	
	datasize[15] = BuildMapData(&OutRom[OFFSET_MAPDATA + 0x8000 + ttlsize], &Levels[6][1], highblock[15], &smi[15]);
	BuildMapPointers(&Levels[6][1], OFFSET_MAPDATA - SIZE_ROM_HEADER + ttlsize, &smi[15], &OutRom[0x8030 + 60]);
	ttlsize += datasize[15];
	
	
	
	
	//	Create things list for each level.  This is currently just a stub
	//	to experiment with the different types of things.
	/*for(echr = 0; echr < 256; echr++)
	{
		OutRom[0xF12E + echr] = Levels[0][0].Things[echr].thingtype;
		if(Levels[0][0].Things[echr].thingtype == 0xFF) { break; }
		else { OutRom[0xF12E + echr] = 0x69; }
	}*/
	
	//	Now onto the actual saving of the file
	FILE *outfile = fopen("blasterout.nes", "w");
	//fputs((char *)&OutRom[0], outfile);
	fwrite((char *)&OutRom[0], SIZE_ROM_HEADER, 1, outfile);
	fwrite((char *)&OutRom[SIZE_ROM_HEADER], SIZE_PRG_BANK, 16, outfile);
	fclose(outfile);
	
	printf("Saved rebuilt ROM\n");
	
	return true;
}

void OutRomAddressToBytes(unsigned short addr, unsigned char * bytes)
{
	bytes[0] = addr & 0xFF;
	bytes[1] = (addr >> 8) & 0xFF;
}

unsigned char MapToBytes(unsigned char * mapdata, Level * level)
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

//unsigned short BuildMapData(unsigned char * mapmeta, Level * level,  unsigned char highblock, unsigned short * blocksize, unsigned short * subblocksize, unsigned short * usbsize, unsigned short * usbattrsize)
unsigned short BuildMapData(unsigned char * mapmeta, Level * level, unsigned char highblock, SerializedMapInfo * smi)
{
	unsigned char highsb = 0x00;
	unsigned char highusb = 0x00;
	
	int pos = 0;
	int x;
	int y;
	
	for(x = 0; x <= highblock; x++)
	{
		for(y = 0; y < 4; y++)
		{
			mapmeta[(x * 4) + y] = (unsigned char)(*level).Blocks[x][y];
			if(mapmeta[(x * 4) + y] > highsb) { highsb = mapmeta[(x * 4) + y]; }
		}
	}
	
	pos = highblock * 4;
	//*blocksize = highblock * 4;
	smi->BlockSize = highblock * 4;
	
	for(x = 0; x <= highsb; x++)
	{
		for(y = 0; y < 4; y++)
		{
			mapmeta[pos + (x * 4) + y] = (unsigned char)(*level).SubBlocks[x][y];
			if(mapmeta[pos + (x * 4) + y] > highusb) { highusb = mapmeta[pos + (x * 4) + y]; }
		}
	}
	
	pos += highsb * 4;
	//*subblocksize = highsb * 4;
	smi->SubBlockSize = highsb * 4;
	
	for(x = 0; x <= highusb; x++)
	{
		for(y = 0; y < 4; y++)
		{
			mapmeta[pos + (x * 4) + y] = (unsigned char)(*level).UltraSubBlocks[x][y];
		}
	}
	
	pos += highusb * 4;
	//*usbsize = highusb * 4;
	smi->USBSize = highusb * 4;
	
	for(x = 0; x <= highusb; x++)
	{
		mapmeta[pos + x] = USBAttributeToByte(&(*level).USBAttributeTable[x]);
	}
	
	pos += highusb;
	//*usbattrsize = highusb;
	smi->USBAttrSize = highusb;
	
	return pos;
}

unsigned char USBAttributeToByte(USBAttributes * usbattr)
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



void BuildMapPointers(Level * level, unsigned short startloc, SerializedMapInfo * smi, unsigned char * outbuf)
{
	//	Build pointers per level/map
	//	The are arranged like:
	//		palette ptr,
	//		usb attr ptr,
	//		usb ptr,
	//		sub block ptr,
	//		block ptr,
	//		map ptr
	unsigned char bytes[2];
	unsigned short addr = startloc;
	
	//	usb attr ptr
	addr = startloc + smi->BlockSize + smi->SubBlockSize + smi->USBSize;
	OutRomAddressToBytes(addr + 0x8000, (unsigned char *)&bytes);
	outbuf[2] = bytes[0];
	outbuf[3] = bytes[1];
	
	//	usb ptr
	addr = startloc + smi->BlockSize + smi->SubBlockSize;
	OutRomAddressToBytes(addr + 0x8000, (unsigned char *)&bytes);
	outbuf[4] = bytes[0];
	outbuf[5] = bytes[1];
	
	//	sub block ptr
	addr = startloc + smi->BlockSize;
	OutRomAddressToBytes(addr + 0x8000, (unsigned char *)&bytes);
	outbuf[6] = bytes[0];
	outbuf[7] = bytes[1];
	
	//	block ptr
	addr = startloc;
	OutRomAddressToBytes(addr + 0x8000, (unsigned char *)&bytes);
	outbuf[8] = bytes[0];
	outbuf[9] = bytes[1];
	
	//	palette: starts at OFFSET_PALETTE - SIZE_ROM_HEADER
	//		If you don't subtract the size of the ROM header before writing the
	//		pointer to the ROM, then the game will look 16 bytes further than
	//		it should for this palette.  It makes the levels look wicked sweet,
	//		but the palettes are in fact corrupted.  This makes it obvious that
	//		a palette editor will need to be created at some point.
	
	if(level->levelid < 5 && level->leveltype == LevelType_Tank)
	{
		//	Areas 1-5 Tank
		addr = level->levelid * 16;
		addr += OFFSET_PALETTE - SIZE_ROM_HEADER;	//	Subtract SIZE_ROM_HEADER
		OutRomAddressToBytes(addr + 0x8000, (unsigned char *)&bytes);
		outbuf[0] = bytes[0];
		outbuf[1] = bytes[1];
		
		addr = (OFFSET_MAP - 0x10) + (1024 * level->levelid);
		OutRomAddressToBytes(addr + 0x8000, (unsigned char *)&bytes);
		outbuf[10] = bytes[0];
		outbuf[11] = bytes[1];
		//printf("Level %d Tank: Map Data Ptr: %02x %02x\n", level->levelid, bytes[0], bytes[1]);
	}
	else if(level->levelid > 4 && level->leveltype == LevelType_Tank)
	{
		//	Areas 5, 6 and 7 Tank
		addr = (level->levelid - 5) * 16;
		addr += OFFSET_PALETTE - SIZE_ROM_HEADER;
		OutRomAddressToBytes(addr + 0x8000, (unsigned char *)&bytes);
		outbuf[0] = bytes[0];
		outbuf[1] = bytes[1];
		
		addr = (OFFSET_MAP - 16) + (1024 * (level->levelid - 5));
		OutRomAddressToBytes(addr + 0x8000, (unsigned char *)&bytes);
		outbuf[10] = bytes[0];
		outbuf[11] = bytes[1];
		//printf("Level %d Tank: Map Data Ptr: %02x %02x\n", level->levelid, bytes[0], bytes[1]);
	}
	else if(level->leveltype == LevelType_Overhead &&
		(level->levelid == 0 || level->levelid == 2))
	{
		//	Areas 1 and 3 Overhead
		if(level->levelid == 0) { addr = 48; } else { addr = 64; }
		addr += OFFSET_PALETTE - SIZE_ROM_HEADER;
		OutRomAddressToBytes(addr + 0x8000, (unsigned char *)&bytes);
		outbuf[0] = bytes[0];
		outbuf[1] = bytes[1];
		
		if(level->levelid == 0) 
		{ 
			addr = (OFFSET_MAP - 16) + (1024 * 3); 
		}
		else 
		{ 
			addr = (OFFSET_MAP - 16) + (1024 * 4); 
		}
		OutRomAddressToBytes(addr + 0x8000, (unsigned char *)&bytes);
		outbuf[10] = bytes[0];
		outbuf[11] = bytes[1];
		
		//printf("Level %d Overhead: Map Data Ptr: %02x %02x\n", level->levelid, bytes[0], bytes[1]);
	}
	else
	{
		unsigned short mult = 0;
		if(level->levelid == 4) { addr = 0; }
		else if(level->levelid == 1) { addr = 16; mult = 1; }
		else if(level->levelid == 5) { addr = 32; mult = 2; }
		else if(level->levelid == 7) { addr = 48; mult = 3; }
		else if(level->levelid == 3) { addr = 64; mult = 4; }
		else if(level->levelid == 6) { addr = 80; mult = 5; }
		
		addr += OFFSET_PALETTE - SIZE_ROM_HEADER;
		OutRomAddressToBytes(addr + 0x8000, (unsigned char *)&bytes);
		outbuf[0] = bytes[0];
		outbuf[1] = bytes[1];
		
		addr = (OFFSET_MAP - 16) + (1024 * mult);
		OutRomAddressToBytes(addr + 0x8000, (unsigned char *)&bytes);
		outbuf[10] = bytes[0];
		outbuf[11] = bytes[1];
		//printf("Level %d Overhead: Map Data Ptr: %02x %02x\n", level->levelid, bytes[0], bytes[1]);
	}
	
}
