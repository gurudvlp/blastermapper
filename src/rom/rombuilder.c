#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "loadrom.h"
#include "saverom.h"
#include "rombuilder.h"

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
	OutRom[SIZE_ROM_HEADER + 0x14 + 0x4000] = 'L';
	OutRom[SIZE_ROM_HEADER + 0x15 + 0x4000] = 'i';
	OutRom[SIZE_ROM_HEADER + 0x16 + 0x4000] = 'n';
	OutRom[SIZE_ROM_HEADER + 0x17 + 0x4000] = 'u';
	OutRom[SIZE_ROM_HEADER + 0x18 + 0x4000] = 's';
	OutRom[SIZE_ROM_HEADER + 0x19 + 0x4000] = 4;	
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
		
		printf("Writing Palette data for level %d:%d at addr:0x%04X\n", cnt, levelorder[cnt], oroff);
		
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
	/*build_SubPalettes((Level *)&Levels[0][0], &OutRom[OFFSET_PALETTE]);
	build_SubPalettes((Level *)&Levels[1][0], &OutRom[OFFSET_PALETTE + SIZE_PALETTE]);
	build_SubPalettes((Level *)&Levels[2][0], &OutRom[OFFSET_PALETTE + (SIZE_PALETTE * 2)]);
	build_SubPalettes((Level *)&Levels[3][0], &OutRom[OFFSET_PALETTE + (SIZE_PALETTE * 3)]);
	build_SubPalettes((Level *)&Levels[4][0], &OutRom[OFFSET_PALETTE + (SIZE_PALETTE * 4)]);
	
	build_SubPalettes((Level *)&Levels[5][0], &OutRom[OFFSET_PALETTE + 0x4000]);
	build_SubPalettes((Level *)&Levels[6][0], &OutRom[OFFSET_PALETTE + 0x4010]);
	build_SubPalettes((Level *)&Levels[7][0], &OutRom[OFFSET_PALETTE + 0x4020]);
	build_SubPalettes((Level *)&Levels[0][1], &OutRom[OFFSET_PALETTE + 0x4030]);
	build_SubPalettes((Level *)&Levels[2][1], &OutRom[OFFSET_PALETTE + 0x4040]);
	
	build_SubPalettes((Level *)&Levels[4][1], &OutRom[OFFSET_PALETTE + 0x8000]);
	build_SubPalettes((Level *)&Levels[1][1], &OutRom[OFFSET_PALETTE + 0x8010]);
	build_SubPalettes((Level *)&Levels[5][1], &OutRom[OFFSET_PALETTE + 0x8020]);
	build_SubPalettes((Level *)&Levels[7][1], &OutRom[OFFSET_PALETTE + 0x8030]);
	build_SubPalettes((Level *)&Levels[3][1], &OutRom[OFFSET_PALETTE + 0x8040]);
	build_SubPalettes((Level *)&Levels[6][1], &OutRom[OFFSET_PALETTE + 0x8050]);*/
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

void build_Maps(unsigned char * OutRom)
{
	
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
	
	for(cnt = 0; cnt < 16; cnt++)
	{
		OutRomAddressToBytes(OFFSET_SCROLLTABLE + 0x8000 - 0x10 + (levelorder[cnt] * 16), (unsigned char *)&bytes);
		OutRom[SIZE_ROM_HEADER + (4 * levinbank) + 2] = bytes[0];
		OutRom[SIZE_ROM_HEADER + (4 * levinbank) + 3] = bytes[1];
		
		//	Because the first two banks each store 5 Areas of data, and the third
		//	bank stores 6 Areas, we have to do a little checking to see where
		//	to save data.
		levinbank++;
		if(cnt == 4) { levinbank = 0; addbank++; }
		if(cnt == 9) { levinbank = 0; addbank++; }
		cnt++;
	}
	
	
	/*
	
	//	Level 1 scroll table pointer
	OutRomAddressToBytes(OFFSET_SCROLLTABLE + 0x8000 - 0x10, (unsigned char *)&bytes);
	OutRom[SIZE_ROM_HEADER + 2] = bytes[0];
	OutRom[SIZE_ROM_HEADER + 3] =  bytes[1];
	
	
	//	Level 6 scroll table pointer
	OutRom[SIZE_ROM_HEADER + 0x4000 + 2] = bytes[0];
	OutRom[SIZE_ROM_HEADER + 0x4000 + 3] = bytes[1];
	
	//	Level 5o scroll table pointer
	OutRom[SIZE_ROM_HEADER + 0x8000 + 2] = bytes[0];
	OutRom[SIZE_ROM_HEADER + 0x8000 + 3] = bytes[1];
	
	//	Level 2 scroll table pointer
	OutRomAddressToBytes(OFFSET_SCROLLTABLE + 16 + 0x8000, (unsigned char *)&bytes);
	OutRom[SIZE_ROM_HEADER + 6] = bytes[0];
	OutRom[SIZE_ROM_HEADER + 7] = bytes[1];
	
	//	Level 7 scroll table pointer
	OutRom[SIZE_ROM_HEADER + 0x4000 + 6] = bytes[0];
	OutRom[SIZE_ROM_HEADER + 0x4000 + 7] = bytes[1];
	
	//	Level 2o scroll table pointer
	OutRom[SIZE_ROM_HEADER + 0x8000 + 6] = bytes[0];
	OutRom[SIZE_ROM_HEADER + 0x8000 + 7] = bytes[1];
	
	//	Level 3 scroll table pointer
	OutRomAddressToBytes(OFFSET_SCROLLTABLE + 32 + 0x8000, (unsigned char *)&bytes);
	OutRom[SIZE_ROM_HEADER + 10] = bytes[0];
	OutRom[SIZE_ROM_HEADER + 11] = bytes[1];
	
	//	Level 8 scroll table pointer
	OutRom[SIZE_ROM_HEADER + 0x4000 + 10] = bytes[0];
	OutRom[SIZE_ROM_HEADER + 0x4000 + 11] = bytes[1];
	
	//	Level 6o scroll table pointer
	OutRom[SIZE_ROM_HEADER + 0x8000 + 10] = bytes[0];
	OutRom[SIZE_ROM_HEADER + 0x8000 + 11] = bytes[1];
	
	//	Level 4 scroll table pointer
	OutRomAddressToBytes(OFFSET_SCROLLTABLE + 48 + 0x8000, (unsigned char *)&bytes);
	OutRom[SIZE_ROM_HEADER + 14] = bytes[0];
	OutRom[SIZE_ROM_HEADER + 15] = bytes[1];
	
	//	Level 1o scroll table pointer
	OutRom[SIZE_ROM_HEADER + 0x4000 + 14] = bytes[0];
	OutRom[SIZE_ROM_HEADER + 0x4000 + 15] = bytes[1];
	
	//	Level 8o scroll table pointer
	OutRom[SIZE_ROM_HEADER + 0x8000 + 14] = bytes[0];
	OutRom[SIZE_ROM_HEADER + 0x8000 + 15] = bytes[1];
	
	//	Level 5 scroll table pointer
	OutRomAddressToBytes(OFFSET_SCROLLTABLE + 64 + 0x8000,(unsigned char *) &bytes);
	OutRom[SIZE_ROM_HEADER + 18] = bytes[0];
	OutRom[SIZE_ROM_HEADER + 19] = bytes[1];
	
	//	Level 3o scroll table pointer
	OutRom[SIZE_ROM_HEADER + 0x4000 + 18] = bytes[0];
	OutRom[SIZE_ROM_HEADER + 0x4000 + 19] = bytes[1];
	
	//	Level 4o scroll table pointer
	OutRom[SIZE_ROM_HEADER + 0x8000 + 18] = bytes[0];
	OutRom[SIZE_ROM_HEADER + 0x8000 + 19] = bytes[0];
	
	//	Level 7o scroll table pointer
	OutRomAddressToBytes(OFFSET_SCROLLTABLE + 80 + 0x8000, (unsigned char *)&bytes);
	OutRom[SIZE_ROM_HEADER + 0x8000 + 22] = bytes[0];
	OutRom[SIZE_ROM_HEADER + 0x8000 + 23] = bytes[1];
	*/
}
