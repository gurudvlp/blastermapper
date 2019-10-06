#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "loadrom.h"
#include "saverom.h"
#include "rombuilder.h"
#include "../level/levelinfo.h"

void build_SubPalettes(Level * level, unsigned char * spbytes);

//	Allocate and initialize memory for building the ROM file.
unsigned char * build_InitializeRom()
{
	int newsize = SIZE_ROM_HEADER + (COUNT_PRG_BANK * SIZE_PRG_BANK) + (COUNT_CHR_BANK * SIZE_CHR_BANK);
	unsigned char * OutRom = (unsigned char *)malloc(newsize);
	
	bzero(OutRom, newsize);
	
	//	Build the ROM Header
	memcpy(OutRom, RomHeader, SIZE_ROM_HEADER);
	
	//	Build CHR ROM
	unsigned int chraddr = SIZE_ROM_HEADER + (COUNT_PRG_BANK * SIZE_PRG_BANK);
	int eb;

	for(eb = 0; eb < COUNT_CHR_BANK; eb++)
	{
		memcpy(&OutRom[chraddr + (eb * SIZE_CHR_BANK)], &ChrRom[eb][0], SIZE_CHR_BANK);
	}
	
	build_InsertWatermarks(OutRom);
	
	
	
	return OutRom;
}

//	Copy banks 3-7
//	Bank 3 includes pointers to things, also some other unknown data.
//	The remaining banks are the programming for the game.
void build_CopyProgramming(unsigned char * OutRom)
{
	int eb;
	for(eb = 3; eb < 8; eb++)
	{
		memcpy(&OutRom[SIZE_ROM_HEADER + (eb * SIZE_PRG_BANK)], &PrgRom[eb][0], SIZE_PRG_BANK);
	}
}

void build_InsertWatermarks(unsigned char * OutRom)
{
	//	Create PRGROM watermark
	int eb;
	
	for(eb = 0; eb < COUNT_PRG_BANK * SIZE_PRG_BANK; eb += 4)
	{
		OutRom[SIZE_ROM_HEADER + eb] = 'g';
		OutRom[SIZE_ROM_HEADER + eb + 1] = 'u';
		OutRom[SIZE_ROM_HEADER + eb + 2] = 'r';
		OutRom[SIZE_ROM_HEADER + eb + 3] = 'u';
	}
	
	//	Watermark :)
	OutRom[SIZE_ROM_HEADER + 0x14] = 'T';
	OutRom[SIZE_ROM_HEADER + 0x15] = 'O';
	OutRom[SIZE_ROM_HEADER + 0x16] = 'A';
	OutRom[SIZE_ROM_HEADER + 0x17] = 'D';
	
	//	Program Version
	OutRom[SIZE_ROM_HEADER + 0x18] = 0x00;
	OutRom[SIZE_ROM_HEADER + 0x19] = 0x02;
	
	//	Second bank watermark
	OutRom[SIZE_ROM_HEADER + 0x14 + OFFSET_BANK_1] = 'L';
	OutRom[SIZE_ROM_HEADER + 0x15 + OFFSET_BANK_1] = 'i';
	OutRom[SIZE_ROM_HEADER + 0x16 + OFFSET_BANK_1] = 'n';
	OutRom[SIZE_ROM_HEADER + 0x17 + OFFSET_BANK_1] = 'u';
	OutRom[SIZE_ROM_HEADER + 0x18 + OFFSET_BANK_1] = 's';
	OutRom[SIZE_ROM_HEADER + 0x19 + OFFSET_BANK_1] = 4;	
}

void build_SpawnPoints(unsigned char * OutRom)
{
	//	Assemble the spawn points
	//	Overhead spawn points start at 0x1C5B2 (OFFSET_SPAWNS_AFTERBOSS)
	//	Tank spawn points start at 0x1CA4B (OFFSET_SPAWNS_TANK)
	int echr;
	
	for(echr = 0; echr < 8; echr++)
	{
		OutRom[OFFSET_SPAWNS_AFTERBOSS + (echr * 2)] = Levels[echr][1].SpawnPoint.x;
		OutRom[OFFSET_SPAWNS_AFTERBOSS + (echr * 2) + 1] = Levels[echr][1].SpawnPoint.y;
		
		OutRom[OFFSET_SPAWNS_TANK + (echr * 2)] = Levels[echr][0].SpawnPoint.x;
		OutRom[OFFSET_SPAWNS_TANK + (echr * 2) + 1] = Levels[echr][0].SpawnPoint.y;
	}
}

void build_Palettes(unsigned char * OutRom)
{
	//	Build the palette info for each level
	//	As mentioned in the Rom Layout document, the areas aren't saved to the
	//	ROM in order.  Because of this, it's difficult to just do a loop to build
	//	each area's palette.  To get around this, we will iterate on an array
	//	of area numbers in the order that they are actually saved.
	
	unsigned short levelorder[] = { 0, 1, 2, 3, 4, 5, 6, 7, 0, 2, 4, 1, 5, 7, 3, 6 };
	unsigned short cnt;
	unsigned short levinbank = 0;
	
	for(cnt = 0; cnt < 16; cnt++)
	{
		unsigned short oroff = levinbank * SIZE_PALETTE;
		if(cnt > 4 && cnt < 10) { oroff += OFFSET_BANK_1; }
		if(cnt >= 10) { oroff += OFFSET_BANK_2; }
		
		oroff += OFFSET_PALETTE;
		
		//printf("Writing Palette data for level %d:%d at addr:0x%04X\n", cnt, levelorder[cnt], oroff);
		
		build_SubPalettes(
			(Level *)&Levels[levelorder[cnt]][(cnt < 8) ? 0 : 1],
			&OutRom[oroff]
		);
		
		//	Because the first two banks each store 5 Areas of data, and the third
		//	bank stores 6 Areas, we have to do a little checking to see where
		//	to save data.
		levinbank++;
		if(cnt == 4) { levinbank = 0; }
		if(cnt == 9) { levinbank = 0; }
	}

}

void build_SubPalettes(Level * level, unsigned char * spbytes)
{
	int esp;
	for(esp = 0; esp < 4; esp++)
	{
		spbytes[esp * 4] = level->SubPalettes[esp].color0;
		spbytes[(esp * 4) + 1] = level->SubPalettes[esp].color1;
		spbytes[(esp * 4) + 2] = level->SubPalettes[esp].color2;
		spbytes[(esp * 4) + 3] = level->SubPalettes[esp].color3;
	}
}

//	Assemble the binary map data.  This will convert the Level structs map
//	data to binary, write it to the OutRom, and also keep track of the highest
//	block id for each Area.
//
//	unsigned char * OutRom
//		The rom being built
//
//	unsigned char * highblock
//		An array of 16 bytes.  Each byte is a value from 0 to 255 that
//		represents the highest block id for each successive Area
void build_Maps(unsigned char * OutRom)
{
	unsigned short levelorder[] = { 0, 1, 2, 3, 4, 5, 6, 7, 0, 2, 4, 1, 5, 7, 3, 6 };
	unsigned short cnt;
	unsigned short levinbank = 0;
	unsigned short addbank = 0;
	unsigned char highblock[16];
	unsigned short ttlsize = 0;
	unsigned short datasize[16];
	
	SerializedMapInfo smi[16];
	
	for(cnt = 0; cnt < 16; cnt++)
	{
		//	Build the 32x32 map grid
		highblock[cnt] = level_MapToBytes(
			&OutRom[OFFSET_MAP + (SIZE_MAP * levinbank) + (addbank * SIZE_PRG_BANK)],
			&Levels[levelorder[cnt]][(cnt < 8) ? 0 : 1]
		);
		
		//	Build all of the blocks, sub blocks and ultra sub blocks
		datasize[cnt] = build_MapData(
			&OutRom[OFFSET_MAPDATA + ttlsize + (addbank * SIZE_PRG_BANK)],
			&Levels[levelorder[cnt]][(cnt < 8) ? 0 : 1],
			highblock[cnt],
			&smi[cnt]
		);
		
		//	Build all of the pointers to the above
		build_MapPointers(
			&Levels[levelorder[cnt]][(cnt < 8) ? 0 : 1],
			OFFSET_MAPDATA - SIZE_ROM_HEADER + ttlsize,
			&smi[cnt],
			&OutRom[OFFSET_LEVELDATAPOINTERS + (SIZE_LEVELDATAPOINTERS * levinbank) + (addbank * SIZE_PRG_BANK)]
		);
		
		ttlsize += datasize[cnt];
		
		levinbank++;
		if(cnt == 4) { levinbank = 0; ttlsize = 0; addbank++; }
		if(cnt == 9) { levinbank = 0; ttlsize = 0; addbank++; }
	}


	////////////////////////////////////////////////////////////////////////////
	//
	//	Why it wouldn't refactor on my first attempt??  Lol
	//
	//	Build map data for each level.  This can be variable sizes, so that
	//	will need to be calculated too.
	//	Max size:
	//		256 blocks * 4 bytes:		1024b
	//		256 subblocks * 4 bytes:	1024b
	//		256 usbs * 4 bytes:			1024b
	//		256 usb attrs * 1 byte:		 256b
	//		Total:						3328b
	
	//	Levels 1-5 tank
	//unsigned short nextstart = 0x1640;
	/*unsigned short ttlsize = 0;
	unsigned short datasize[16];
	
	SerializedMapInfo smi[16];
	
	levinbank = 0;
	addbank = 0;
	
	for(cnt = 0; cnt < 16; cnt++)
	{
		datasize[cnt] = build_MapData(
			&OutRom[OFFSET_MAPDATA + ttlsize + (addbank * SIZE_PRG_BANK)],
			&Levels[levelorder[cnt]][(cnt < 8) ? 0 : 1],
			highblock[cnt],
			&smi[cnt]
		);
		
		build_MapPointers(
			&Levels[levelorder[cnt]][(cnt < 8) ? 0 : 1],
			OFFSET_MAPDATA - SIZE_ROM_HEADER + ttlsize,
			&smi[cnt],
			&OutRom[OFFSET_LEVELDATAPOINTERS + (SIZE_LEVELDATAPOINTERS * levinbank) + (addbank * SIZE_PRG_BANK)]
		);
		
		ttlsize += datasize[cnt];
		levinbank++;
		
		if(cnt == 4) { ttlsize = 0; addbank++; levinbank = 0; }
		if(cnt == 9) { ttlsize = 0; addbank++; levinbank = 0; }
	}*/
	
}

void build_ScrollTables(unsigned char * OutRom)
{
	//	The levels in the game aren't saved in order, so here is an array that
	//	specifies the order in which they are saved.
	unsigned short levelorder[] = { 0, 1, 2, 3, 4, 5, 6, 7, 0, 2, 4, 1, 5, 7, 3, 6 };
	
	unsigned short cnt;
	unsigned short levinbank = 0;
	unsigned char bytes[2];
	unsigned short addbank = 0;
	unsigned short eb;
	
	//	First, build all of the scroll table pointers.
	
	for(cnt = 0; cnt < 16; cnt++)
	{
		OutRomAddressToBytes(
			OFFSET_SCROLLTABLE + 0x8000 - SIZE_ROM_HEADER + (levinbank * 16)/*(levelorder[cnt] * levinbank)*/,
			(unsigned char *)&bytes
		);
		
		OutRom[SIZE_ROM_HEADER + (4 * levinbank) + (SIZE_PRG_BANK * addbank) + 2] = bytes[0];
		OutRom[SIZE_ROM_HEADER + (4 * levinbank) + (SIZE_PRG_BANK * addbank) + 3] = bytes[1];
		
		//	Because the first two banks each store 5 Areas of data, and the third
		//	bank stores 6 Areas, we have to do a little checking to see where
		//	to save data.
		levinbank++;
		if(cnt == 4) { levinbank = 0; addbank++; }
		if(cnt == 9) { levinbank = 0; addbank++; }
	}
	
	//	Now, the actual scroll tables need to be built.
	levinbank = 0;
	addbank = 0;
	
	for(cnt = 0; cnt < 16; cnt++)
	{
		for(eb = 0; eb < SIZE_SCROLLTABLE; eb++)
		{
			OutRom[OFFSET_SCROLLTABLE + (SIZE_SCROLLTABLE * levinbank) + (SIZE_PRG_BANK * addbank) + eb] = 
				Levels[levelorder[cnt]][(cnt < 8) ? 0 : 1].ScrollTable[eb];
		}
		
		levinbank++;
		if(cnt == 4) { levinbank = 0; addbank++; }
		if(cnt == 9) { levinbank = 0; addbank++; }
	}

}

//	Assemble all of the data pointers for each level.  The location of the
//	pointers is hard coded into the ROM, but where they point to is a little bit
//	more flexible.  The layout of the rebuilt ROM is described in the Rom Layout
//	document.
void build_LevelDataPointers(unsigned char * OutRom)
{
	unsigned short cnt;
	unsigned short levinbank = 0;
	unsigned char bytes[2];
	unsigned short addbank = 0;
	char txt[64];
	
	for(cnt = 0; cnt < 16; cnt++)
	{
		OutRomAddressToBytes(
			(OFFSET_LEVELDATAPOINTERS - SIZE_ROM_HEADER + 0x8000) + (levinbank * 12),
			(unsigned char *)&bytes
		);
		
		//	Each level has two pointers stored at the start of the ROM.  There
		//	is the pointer to the actual data pointers, and there is a pointer
		//	to the scroll table.  This leaves us with (level in bank * 4 bytes)
		OutRom[(4 * levinbank) + (addbank * SIZE_PRG_BANK) + SIZE_ROM_HEADER] = bytes[0];
		OutRom[(4 * levinbank) + (addbank * SIZE_PRG_BANK) + SIZE_ROM_HEADER + 1] = bytes[1];
		

		sprintf(txt, "bank: %d   levinbank: %d", addbank, levinbank);
		PrintLevelPointer(txt, (unsigned char *)&bytes);
		
		levinbank++;
		if(cnt == 4) { levinbank = 0; addbank++; }
		if(cnt == 9) { levinbank = 0; addbank++; }
	}
	
	
}


unsigned short build_MapData(unsigned char * mapmeta, Level * level, unsigned char highblock, SerializedMapInfo * smi)
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
		mapmeta[pos + x] = level_USBAttributeToByte(&(*level).USBAttributeTable[x]);
	}
	
	pos += highusb;
	//*usbattrsize = highusb;
	smi->USBAttrSize = highusb;
	
	return pos;
}

void build_MapPointers(Level * level, unsigned short startloc, SerializedMapInfo * smi, unsigned char * outbuf)
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
		addr = level->levelid * SIZE_PALETTE;
		addr += OFFSET_PALETTE - SIZE_ROM_HEADER;	//	Subtract SIZE_ROM_HEADER
		OutRomAddressToBytes(addr + 0x8000, (unsigned char *)&bytes);
		outbuf[0] = bytes[0];
		outbuf[1] = bytes[1];
		
		addr = (OFFSET_MAP - SIZE_ROM_HEADER) + (SIZE_MAP * level->levelid);
		OutRomAddressToBytes(addr + 0x8000, (unsigned char *)&bytes);
		outbuf[10] = bytes[0];
		outbuf[11] = bytes[1];
		//printf("Level %d Tank: Map Data Ptr: %02x %02x\n", level->levelid, bytes[0], bytes[1]);
	}
	else if(level->levelid > 4 && level->leveltype == LevelType_Tank)
	{
		//	Areas 5, 6 and 7 Tank
		addr = (level->levelid - 5) * SIZE_PALETTE;
		addr += OFFSET_PALETTE - SIZE_ROM_HEADER;
		OutRomAddressToBytes(addr + 0x8000, (unsigned char *)&bytes);
		outbuf[0] = bytes[0];
		outbuf[1] = bytes[1];
		
		addr = (OFFSET_MAP - SIZE_ROM_HEADER) + (SIZE_MAP * (level->levelid - 5));
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
			addr = (OFFSET_MAP - SIZE_ROM_HEADER) + (SIZE_MAP * 3); 
		}
		else 
		{ 
			addr = (OFFSET_MAP - SIZE_ROM_HEADER) + (SIZE_MAP * 4); 
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
		
		addr = (OFFSET_MAP - SIZE_ROM_HEADER) + (SIZE_MAP * mult);
		OutRomAddressToBytes(addr + 0x8000, (unsigned char *)&bytes);
		outbuf[10] = bytes[0];
		outbuf[11] = bytes[1];
		//printf("Level %d Overhead: Map Data Ptr: %02x %02x\n", level->levelid, bytes[0], bytes[1]);
	}
	
}
