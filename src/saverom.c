#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "loadrom.h"
#include "saverom.h"

bool SaveRom()
{
	//	Here goes nothing..
	//	Save the rebuilt ROM
	
	int eb;
	unsigned short echr;
	
	int newsize = SIZE_ROM_HEADER + (COUNT_PRG_BANK * SIZE_PRG_BANK) + (COUNT_CHR_BANK * SIZE_CHR_BANK);
	unsigned char OutRom[newsize];
	bzero(&OutRom, sizeof(OutRom));
	
	//	Build the ROM Header
	memcpy(OutRom, RomHeader, SIZE_ROM_HEADER);
	
	//	Build CHR ROM
	unsigned int chraddr = SIZE_ROM_HEADER + (COUNT_PRG_BANK * SIZE_PRG_BANK);

	for(eb = 0; eb < COUNT_CHR_BANK; eb++)
	{
		memcpy(&OutRom[chraddr + (eb * SIZE_CHR_BANK)], &ChrRom[eb][0], SIZE_CHR_BANK);
	}
	
	//	Create PRGROM watermark
	unsigned short prgaddr = SIZE_ROM_HEADER;
	for(eb = 0; eb < COUNT_PRG_BANK * SIZE_PRG_BANK; eb += 4)
	{
		OutRom[eb + prgaddr] = 'g';
		OutRom[eb + prgaddr + 1] = 'u';
		OutRom[eb + prgaddr + 2] = 'r';
		OutRom[eb + prgaddr + 3] = 'u';
	}
	
	//	Copy banks 3-7
	//	Bank 3 includes pointers to things, also some other unknown data.
	//	The remaining banks are the programming for the game.
	for(eb = 3; eb < 8; eb++)
	{
		memcpy(&OutRom[prgaddr + (eb * SIZE_PRG_BANK)], &PrgRom[eb][0], SIZE_PRG_BANK);
	}
	
	//	Now the actual building of the PRGROM
	//	20 bytes of level pointers
	//		[level][scrolltable]
	//	Level 1:
	//		12b:[datapointers]
	//			palette
	//			usb attribute table
	//			usb table
	//			sb table
	//			block table
	//			map
	//
	//	0x00 [lvlptr / scrollptr]
	//	0x14 TOAD
	//	0x18 {0}{1}
	//	0x20 level 1 pointers
	//	0x2C level 2 pointers
	//	0x38 level 3 pointers
	//	0xxx level 4 pointers
	//	0xxx level 5 pointers
	//	0x92 level 1 palette
	//	0xA2 level 2 palette
	//	0xB2 level 3 palette
	//	0xC2 level 4 palette
	//	0xD2 level 5 palette
	//	0xE2 level 1 scroll table
	//	0xF2 level 2 scroll table
	//	0x102 level 3 scroll table
	//	0x112 level 4 scroll table
	//	0x122 level 5 scroll table
	//	0x132 level 1 map (1024 bytes)
	//	0x532 level 2 map
	//	0x932 level 3 map
	//	0xD32 level 4 map
	//	0x1232 level 5 map
	//	0x1632 gurutronik
	//	0x1640 block/subblock/ultrasubblock/usbattr
	//
	//	Levels 6-8, overhead 1 & 3
	//
	//	same as above, but + 0x4000
	//	Levels 2, 5-8 overhead
	//
	//	Scroll table starts at 0x100 + 0x8000
	//	Maps start at 0x160 + 0x8000
	//	Map data starts at 0x1960 + 0x8000 (??  0x1C60 maybe?)
	
	Out_PaletteOffset = 0x0092 + prgaddr;
	Out_MapOffset = 0x0132 + prgaddr;
	Out_OvMapOffset = 0x0160 + prgaddr;
	Out_MapDataOffset = 0x1640 + prgaddr;
	Out_OvMapDataOffset = 0x1C60 + prgaddr;
	Out_LevelPointersOffset = 0x0020 + prgaddr;
	Out_ROMLevelPointerAddr = 0x8020;
	
	//	Watermark :)
	OutRom[prgaddr + 0x14] = 'T';
	OutRom[prgaddr + 0x15] = 'O';
	OutRom[prgaddr + 0x16] = 'A';
	OutRom[prgaddr + 0x17] = 'D';
	
	//	Program Version
	OutRom[prgaddr + 0x18] = 0x00;
	OutRom[prgaddr + 0x19] = 0x02;
	
	//	Second bank watermark
	OutRom[prgaddr + 0x14 + 0x4000] = 'L';
	OutRom[prgaddr + 0x15 + 0x4000] = 'i';
	OutRom[prgaddr + 0x16 + 0x4000] = 'n';
	OutRom[prgaddr + 0x17 + 0x4000] = 'u';
	OutRom[prgaddr + 0x18 + 0x4000] = 's';
	OutRom[prgaddr + 0x19 + 0x4000] = 4;
	
	//unsigned short lvlptraddr = 0x20;
	//unsigned short dataaddr = lvlptraddr + (12 * 5);
	
	//printf("Level data can start at 0x%04x (%d)\n", dataaddr, dataaddr);
	
	//	Start with each set of data pointers for level data
	//	overhead levels 2, 5-8 are in bank 2,
	//	ordered like 5, 2, 6, 8, 4, 7
	
	unsigned char bytes[2];
	
	//	Level 1 data pointer
	OutRomAddressToBytes(0x20 + 0x8000, (unsigned char *)&bytes);
	OutRom[prgaddr] = bytes[0];
	OutRom[prgaddr + 1] = bytes[1];
	PrintLevelPointer("Level 1 Tank", (unsigned char *)&bytes);
	
	//	Level 6 data pointer
	OutRom[prgaddr + 0x4000] = bytes[0];
	OutRom[prgaddr + 0x4000 + 1] = bytes[1];
	PrintLevelPointer("Level 6 Tank", (unsigned char *)&bytes);
	
	//	Level 5o data pointer
	OutRom[prgaddr + 0x8000] = bytes[0];
	OutRom[prgaddr + 0x8000 + 1] = bytes[1];
	PrintLevelPointer("Level 5 Overhead", (unsigned char *)&bytes);
	
	//	Level 2 data pointer
	OutRomAddressToBytes(0x20 + 12 + 0x8000, (unsigned char *)&bytes);
	OutRom[prgaddr + 4] = bytes[0];
	OutRom[prgaddr + 5] = bytes[1];
	PrintLevelPointer("Level 2 Tank", (unsigned char *)&bytes);
	
	//	Level 7 data pointer
	OutRom[prgaddr + 0x4000 + 4] = bytes[0];
	OutRom[prgaddr + 0x4000 + 5] = bytes[1];
	PrintLevelPointer("Level 7 Tank", (unsigned char *)&bytes);
	
	//	Level 2o data pointer
	OutRom[prgaddr + 0x8000 + 4] = bytes[0];
	OutRom[prgaddr + 0x8000 + 5] = bytes[1];
	PrintLevelPointer("Level 2 Overhead", (unsigned char *)&bytes);
	
	//	Level 3 data pointer
	OutRomAddressToBytes(0x20 + 24 + 0x8000, (unsigned char *)&bytes);
	OutRom[prgaddr + 8] = bytes[0];
	OutRom[prgaddr + 9] = bytes[1];
	PrintLevelPointer("Level 3 Tank", (unsigned char *)&bytes);
	
	//	Level 8 data pointer
	OutRom[prgaddr + 0x4000 + 8] = bytes[0];
	OutRom[prgaddr + 0x4000 + 9] = bytes[1];
	PrintLevelPointer("Level 8 Tank", (unsigned char *)&bytes);
	
	//	Level 6o data pointer
	OutRom[prgaddr + 0x8000 + 8] = bytes[0];
	OutRom[prgaddr + 0x8000 + 9] = bytes[1];
	PrintLevelPointer("Level 6 Overhead", (unsigned char *)&bytes);
	
	//	Level 4 data pointer
	OutRomAddressToBytes(0x20 + 36 + 0x8000, (unsigned char *)&bytes);
	OutRom[prgaddr + 12] = bytes[0];
	OutRom[prgaddr + 13] = bytes[1];
	PrintLevelPointer("Level 4 Tank", (unsigned char *)&bytes);
	
	//	Level 1o data pointer
	OutRom[prgaddr + 0x4000 + 12] = bytes[0];
	OutRom[prgaddr + 0x4000 + 13] = bytes[1];
	PrintLevelPointer("Level 4 Overhead", (unsigned char *)&bytes);
	
	//	Level 8o data pointer
	OutRom[prgaddr + 0x8000 + 12] = bytes[0];
	OutRom[prgaddr + 0x8000 + 13] = bytes[1];
	PrintLevelPointer("Level 8 Overhead", (unsigned char *)&bytes);
	
	//	Level 5 data pointer
	OutRomAddressToBytes(0x20 + 48 + 0x8000, (unsigned char *)&bytes);
	OutRom[prgaddr + 16] = bytes[0];
	OutRom[prgaddr + 17] = bytes[1];
	PrintLevelPointer("Level 5 Tank", (unsigned char *)&bytes);
	
	//	Level 3o data pointer
	OutRom[prgaddr + 0x4000 + 16] = bytes[0];
	OutRom[prgaddr + 0x4000 + 17] = bytes[0];
	PrintLevelPointer("Level 3 Overhead", (unsigned char *)&bytes);
	
	//	Level 4o data pointer
	OutRom[prgaddr + 0x8000 + 16] = bytes[0];
	OutRom[prgaddr + 0x8000 + 17] = bytes[1];
	PrintLevelPointer("Level 4 Overhead", (unsigned char *)&bytes);
	
	//	Level 7o data pointer
	OutRomAddressToBytes(0x20 + 60 + 0x8000, (unsigned char *)&bytes);
	OutRom[prgaddr + 0x8000 + 20] = bytes[0];
	OutRom[prgaddr + 0x8000 + 21] = bytes[1];
	PrintLevelPointer("Level 7 Overhead", (unsigned char *)&bytes);
	
	//	Level 1 scroll table pointer
	OutRomAddressToBytes(0xE2 + 0x8000, (unsigned char *)&bytes);
	OutRom[prgaddr + 2] = bytes[0];
	OutRom[prgaddr + 3] =  bytes[1];
	
	//	Level 6 scroll table pointer
	OutRom[prgaddr + 0x4000 + 2] = bytes[0];
	OutRom[prgaddr + 0x4000 + 3] = bytes[1];
	
	//	Level 5o scroll table pointer
	OutRom[prgaddr + 0x8000 + 2] = bytes[0];
	OutRom[prgaddr + 0x8000 + 3] = bytes[1];
	
	//	Level 2 scroll table pointer
	OutRomAddressToBytes(0xE2 + 16 + 0x8000, (unsigned char *)&bytes);
	OutRom[prgaddr + 6] = bytes[0];
	OutRom[prgaddr + 7] = bytes[1];
	
	//	Level 7 scroll table pointer
	OutRom[prgaddr + 0x4000 + 6] = bytes[0];
	OutRom[prgaddr + 0x4000 + 7] = bytes[1];
	
	//	Level 2o scroll table pointer
	OutRom[prgaddr + 0x8000 + 6] = bytes[0];
	OutRom[prgaddr + 0x8000 + 7] = bytes[1];
	
	//	Level 3 scroll table pointer
	OutRomAddressToBytes(0xE2 + 32 + 0x8000, (unsigned char *)&bytes);
	OutRom[prgaddr + 10] = bytes[0];
	OutRom[prgaddr + 11] = bytes[1];
	
	//	Level 8 scroll table pointer
	OutRom[prgaddr + 0x4000 + 10] = bytes[0];
	OutRom[prgaddr + 0x4000 + 11] = bytes[1];
	
	//	Level 6o scroll table pointer
	OutRom[prgaddr + 0x8000 + 10] = bytes[0];
	OutRom[prgaddr + 0x8000 + 11] = bytes[1];
	
	//	Level 4 scroll table pointer
	OutRomAddressToBytes(0xE2 + 48 + 0x8000, (unsigned char *)&bytes);
	OutRom[prgaddr + 14] = bytes[0];
	OutRom[prgaddr + 15] = bytes[1];
	
	//	Level 1o scroll table pointer
	OutRom[prgaddr + 0x4000 + 14] = bytes[0];
	OutRom[prgaddr + 0x4000 + 15] = bytes[1];
	
	//	Level 8o scroll table pointer
	OutRom[prgaddr + 0x8000 + 14] = bytes[0];
	OutRom[prgaddr + 0x8000 + 15] = bytes[1];
	
	//	Level 5 scroll table pointer
	OutRomAddressToBytes(0xE2 + 64 + 0x8000,(unsigned char *) &bytes);
	OutRom[prgaddr + 18] = bytes[0];
	OutRom[prgaddr + 19] = bytes[1];
	
	//	Level 3o scroll table pointer
	OutRom[prgaddr + 0x4000 + 18] = bytes[0];
	OutRom[prgaddr + 0x4000 + 19] = bytes[1];
	
	//	Level 4o scroll table pointer
	OutRom[prgaddr + 0x8000 + 18] = bytes[0];
	OutRom[prgaddr + 0x8000 + 19] = bytes[0];
	
	//	Level 7o scroll table pointer
	OutRomAddressToBytes(0xE2 + 80 + 0x8000, (unsigned char *)&bytes);
	OutRom[prgaddr + 0x8000 + 22] = bytes[0];
	OutRom[prgaddr + 0x8000 + 23] = bytes[1];
	
	
	//	Now to build the scroll tables for each level
	
	//	Levels 1-5 tank
	for(echr = 0; echr < 5; echr++)
	{
		for(eb = 0; eb < 16; eb++)
		{
			OutRom[prgaddr + 0xE2 + (16 * echr) + eb] = Levels[echr][0].ScrollTable[eb];
		}
	}
	
	//	Levels 6-8 tank
	for(echr = 0; echr < 3; echr++)
	{
		for(eb = 0; eb < 16; eb++)
		{
			OutRom[prgaddr + 0x40E2 + (16 * echr) + eb] = Levels[echr + 5][0].ScrollTable[eb];
		}
	}
	
	//	Levels 1 & 3 overhead
	for(eb = 0; eb < 16; eb++)
	{
		OutRom[prgaddr + 0x40E2 + 48 + eb] = Levels[0][1].ScrollTable[eb];
		OutRom[prgaddr + 0x40E2 + 64 + eb] = Levels[2][1].ScrollTable[eb];
	}
	
	//	Levels 2, 5-8 overhead
	for(eb = 0; eb < 16; eb++)
	{
		OutRom[prgaddr + 0x8160 + eb] = Levels[4][1].ScrollTable[eb];
		OutRom[prgaddr + 0x8160 + eb + 16] = Levels[1][1].ScrollTable[eb];
		OutRom[prgaddr + 0x8160 + eb + 32] = Levels[5][1].ScrollTable[eb];
		OutRom[prgaddr + 0x8160 + eb + 48] = Levels[7][1].ScrollTable[eb];
		OutRom[prgaddr + 0x8160 + eb + 64] = Levels[3][1].ScrollTable[eb];
		OutRom[prgaddr + 0x8160 + eb + 80] = Levels[6][1].ScrollTable[eb];
	}
	
	//	Build the palette info for each level
	
	BuildSubPalettes((Level *)&Levels[0][0], &OutRom[Out_PaletteOffset]);
	BuildSubPalettes((Level *)&Levels[1][0], &OutRom[Out_PaletteOffset + 16]);
	BuildSubPalettes((Level *)&Levels[2][0], &OutRom[Out_PaletteOffset + 32]);
	BuildSubPalettes((Level *)&Levels[3][0], &OutRom[Out_PaletteOffset + 48]);
	BuildSubPalettes((Level *)&Levels[4][0], &OutRom[Out_PaletteOffset + 64]);
	
	BuildSubPalettes((Level *)&Levels[5][0], &OutRom[Out_PaletteOffset + 0x4000]);
	BuildSubPalettes((Level *)&Levels[6][0], &OutRom[Out_PaletteOffset + 0x4010]);
	BuildSubPalettes((Level *)&Levels[7][0], &OutRom[Out_PaletteOffset + 0x4020]);
	BuildSubPalettes((Level *)&Levels[0][1], &OutRom[Out_PaletteOffset + 0x4030]);
	BuildSubPalettes((Level *)&Levels[2][1], &OutRom[Out_PaletteOffset + 0x4040]);
	
	BuildSubPalettes((Level *)&Levels[4][1], &OutRom[Out_PaletteOffset + 0x8000]);
	BuildSubPalettes((Level *)&Levels[1][1], &OutRom[Out_PaletteOffset + 0x8010]);
	BuildSubPalettes((Level *)&Levels[5][1], &OutRom[Out_PaletteOffset + 0x8020]);
	BuildSubPalettes((Level *)&Levels[7][1], &OutRom[Out_PaletteOffset + 0x8030]);
	BuildSubPalettes((Level *)&Levels[3][1], &OutRom[Out_PaletteOffset + 0x8040]);
	BuildSubPalettes((Level *)&Levels[6][1], &OutRom[Out_PaletteOffset + 0x8050]);
	

	
	//	Onto the map.  We need the actual map data, and also the highest block id
	unsigned char highblock[16];
	
	unsigned short datasize[16];
	
	highblock[0] = MapToBytes(&OutRom[Out_MapOffset], &Levels[0][0]);
	highblock[1] = MapToBytes(&OutRom[Out_MapOffset + 0x0400], &Levels[1][0]);
	highblock[2] = MapToBytes(&OutRom[Out_MapOffset + 0x0800], &Levels[2][0]);
	highblock[3] = MapToBytes(&OutRom[Out_MapOffset + 0x0C00], &Levels[3][0]);
	highblock[4] = MapToBytes(&OutRom[Out_MapOffset + 0x1000], &Levels[4][0]);
	
	highblock[5] = MapToBytes(&OutRom[Out_MapOffset + 0x4000], &Levels[5][0]);
	highblock[6] = MapToBytes(&OutRom[Out_MapOffset + 0x4400], &Levels[6][0]);
	highblock[7] = MapToBytes(&OutRom[Out_MapOffset + 0x4800], &Levels[7][0]);
	highblock[8] = MapToBytes(&OutRom[Out_MapOffset + 0x4C00], &Levels[0][1]);
	highblock[9] = MapToBytes(&OutRom[Out_MapOffset + 0x5000], &Levels[2][1]);
	
	highblock[10] = MapToBytes(&OutRom[Out_OvMapOffset + 0x8000], &Levels[4][1]);
	highblock[11] = MapToBytes(&OutRom[Out_OvMapOffset + 0x8400], &Levels[1][1]);
	highblock[12] = MapToBytes(&OutRom[Out_OvMapOffset + 0x8800], &Levels[5][1]);
	highblock[13] = MapToBytes(&OutRom[Out_OvMapOffset + 0x8C00], &Levels[7][1]);
	highblock[14] = MapToBytes(&OutRom[Out_OvMapOffset + 0x9000], &Levels[3][1]);
	highblock[15] = MapToBytes(&OutRom[Out_OvMapOffset + 0x9400], &Levels[6][1]);
	
	
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
	datasize[0] = BuildMapData(&OutRom[Out_MapDataOffset], &Levels[0][0], highblock[0], &smi[0]);
	BuildMapPointers(&Levels[0][0], 0x1640, &smi[0], &OutRom[0x30]);
	ttlsize += datasize[0];
	
	datasize[1] = BuildMapData(&OutRom[Out_MapDataOffset + ttlsize], &Levels[1][0], highblock[1], &smi[1]);
	BuildMapPointers(&Levels[1][0], 0x1640 + ttlsize, &smi[1], &OutRom[0x30 + 12]);
	ttlsize += datasize[1];
	
	datasize[2] = BuildMapData(&OutRom[Out_MapDataOffset + ttlsize], &Levels[2][0], highblock[2], &smi[2]);
	BuildMapPointers(&Levels[2][0], 0x1640 + ttlsize, &smi[2], &OutRom[0x30 + 24]);
	ttlsize += datasize[2];
	
	datasize[3] = BuildMapData(&OutRom[Out_MapDataOffset + ttlsize], &Levels[3][0], highblock[3], &smi[3]);
	BuildMapPointers(&Levels[3][0], 0x1640 + ttlsize, &smi[3], &OutRom[0x30 + 36]);
	ttlsize += datasize[3];
	
	datasize[4] = BuildMapData(&OutRom[Out_MapDataOffset + ttlsize], &Levels[4][0], highblock[4], &smi[4]);
	BuildMapPointers(&Levels[4][0], 0x1640 + ttlsize, &smi[4], &OutRom[0x30 + 48]);
	ttlsize += datasize[4];
	
	//	Levels 6-8 tank
	ttlsize = 0;
	datasize[5] = BuildMapData(&OutRom[Out_MapDataOffset + 0x4000], &Levels[5][0], highblock[5], &smi[5]);
	BuildMapPointers(&Levels[5][0], 0x1640 + ttlsize, &smi[5], &OutRom[0x4030]);
	ttlsize += datasize[5];
	
	datasize[6] = BuildMapData(&OutRom[Out_MapDataOffset + 0x4000 + ttlsize], &Levels[6][0], highblock[6], &smi[6]);
	BuildMapPointers(&Levels[6][0], 0x1640 + ttlsize, &smi[6], &OutRom[0x4030 + 12]);
	ttlsize += datasize[6];
	
	datasize[7] = BuildMapData(&OutRom[Out_MapDataOffset + 0x4000 + ttlsize], &Levels[7][0], highblock[7], &smi[7]);
	BuildMapPointers(&Levels[7][0], 0x1640 + ttlsize, &smi[7], &OutRom[0x4030 + 24]);
	ttlsize += datasize[7];
	
	//	Levels 1, 3 overhead
	datasize[8] = BuildMapData(&OutRom[Out_MapDataOffset + 0x4000 + ttlsize], &Levels[0][1], highblock[8], &smi[8]);
	BuildMapPointers(&Levels[0][1], 0x1640 + ttlsize, &smi[8], &OutRom[0x4030 + 36]);
	ttlsize += datasize[8];
	
	datasize[9] = BuildMapData(&OutRom[Out_MapDataOffset + 0x4000 + ttlsize], &Levels[2][1], highblock[9], &smi[9]);
	BuildMapPointers(&Levels[2][1], 0x1640 + ttlsize, &smi[9], &OutRom[0x4030 + 48]);
	ttlsize += datasize[9];
	
	
	//	Levels 2, 4-8 overhead
	ttlsize = 0;
	datasize[10] = BuildMapData(&OutRom[Out_OvMapDataOffset + 0x8000], &Levels[4][1], highblock[10], &smi[10]);
	BuildMapPointers(&Levels[4][1], 0x1C60 + ttlsize, &smi[10], &OutRom[0x8030]);
	ttlsize += datasize[10];
	
	datasize[11] = BuildMapData(&OutRom[Out_OvMapDataOffset + 0x8000 + ttlsize], &Levels[1][1], highblock[11], &smi[11]);
	BuildMapPointers(&Levels[1][1], 0x1C60 + ttlsize, &smi[11], &OutRom[0x8030 + 12]);
	ttlsize += datasize[11];
	
	datasize[12] = BuildMapData(&OutRom[Out_OvMapDataOffset + 0x8000 + ttlsize], &Levels[5][1], highblock[12], &smi[12]);
	BuildMapPointers(&Levels[5][1], 0x1C60 + ttlsize, &smi[12], &OutRom[0x8030 + 24]);
	ttlsize += datasize[12];
	
	datasize[13] = BuildMapData(&OutRom[Out_OvMapDataOffset + 0x8000 + ttlsize], &Levels[7][1], highblock[13], &smi[13]);
	BuildMapPointers(&Levels[7][1], 0x1C60 + ttlsize, &smi[13], &OutRom[0x8030 + 36]);
	ttlsize += datasize[13];
	
	datasize[14] = BuildMapData(&OutRom[Out_OvMapDataOffset + 0x8000 + ttlsize], &Levels[3][1], highblock[14], &smi[14]);
	BuildMapPointers(&Levels[3][1], 0x1C60 + ttlsize, &smi[14], &OutRom[0x8030 + 48]);
	ttlsize += datasize[14];
	
	datasize[15] = BuildMapData(&OutRom[Out_OvMapDataOffset + 0x8000 + ttlsize], &Levels[6][1], highblock[15], &smi[15]);
	BuildMapPointers(&Levels[6][1], 0x1C60 + ttlsize, &smi[15], &OutRom[0x8030 + 60]);
	ttlsize += datasize[15];
	
	
	//	Assemble the spawn points
	//	Overhead spawn points start at 0x1C5B2 (OFFSET_SPAWNS_AFTERBOSS)
	//	Tank spawn points start at 0x1CA4B (OFFSET_SPAWNS_TANK)
	for(echr = 0; echr < 8; echr++)
	{
		OutRom[OFFSET_SPAWNS_AFTERBOSS + (echr * 2)] = Levels[echr][1].SpawnPoint.x;
		OutRom[OFFSET_SPAWNS_AFTERBOSS + (echr * 2) + 1] = Levels[echr][1].SpawnPoint.y;
		
		OutRom[OFFSET_SPAWNS_TANK + (echr * 2)] = Levels[echr][0].SpawnPoint.x;
		OutRom[OFFSET_SPAWNS_TANK + (echr * 2) + 1] = Levels[echr][0].SpawnPoint.y;
	}
	
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

void BuildSubPalettes(Level * level, unsigned char * spbytes)
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
	
	//	palette (starts at 0x0092
	if(level->levelid < 5 && level->leveltype == LevelType_Tank)
	{
		//	Areas 1-5 Tank
		addr = level->levelid * 16;
		addr += 0x0092;
		OutRomAddressToBytes(addr + 0x8000, (unsigned char *)&bytes);
		outbuf[0] = bytes[0];
		outbuf[1] = bytes[1];
		
		addr = (Out_MapOffset - 0x10) + (1024 * level->levelid);
		OutRomAddressToBytes(addr + 0x8000, (unsigned char *)&bytes);
		outbuf[10] = bytes[0];
		outbuf[11] = bytes[1];
		printf("Level %d Tank: Map Data Ptr: %02x %02x\n", level->levelid, bytes[0], bytes[1]);
	}
	else if(level->levelid > 4 && level->leveltype == LevelType_Tank)
	{
		//	Areas 5, 6 and 7 Tank
		addr = (level->levelid - 5) * 16;
		addr += 0x0092;
		OutRomAddressToBytes(addr + 0x8000, (unsigned char *)&bytes);
		outbuf[0] = bytes[0];
		outbuf[1] = bytes[1];
		
		addr = (Out_MapOffset - 16) + (1024 * (level->levelid - 5));
		OutRomAddressToBytes(addr + 0x8000, (unsigned char *)&bytes);
		outbuf[10] = bytes[0];
		outbuf[11] = bytes[1];
		printf("Level %d Tank: Map Data Ptr: %02x %02x\n", level->levelid, bytes[0], bytes[1]);
	}
	else if(level->leveltype == LevelType_Overhead &&
		(level->levelid == 0 || level->levelid == 2))
	{
		//	Areas 1 and 3 Overhead
		if(level->levelid == 0) { addr = 48; } else { addr = 64; }
		addr += 0x0092;
		OutRomAddressToBytes(addr + 0x8000, (unsigned char *)&bytes);
		outbuf[0] = bytes[0];
		outbuf[1] = bytes[1];
		
		if(level->levelid == 0) 
		{ 
			addr = (Out_MapOffset - 16) + (1024 * 3); 
		}
		else 
		{ 
			addr = (Out_MapOffset - 16) + (1024 * 4); 
		}
		OutRomAddressToBytes(addr + 0x8000, (unsigned char *)&bytes);
		outbuf[10] = bytes[0];
		outbuf[11] = bytes[1];
		
		printf("Level %d Overhead: Map Data Ptr: %02x %02x\n", level->levelid, bytes[0], bytes[1]);
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
		
		addr += 0x0092;
		OutRomAddressToBytes(addr + 0x8000, (unsigned char *)&bytes);
		outbuf[0] = bytes[0];
		outbuf[1] = bytes[1];
		
		addr = (Out_OvMapOffset - 16) + (1024 * mult);
		OutRomAddressToBytes(addr + 0x8000, (unsigned char *)&bytes);
		outbuf[10] = bytes[0];
		outbuf[11] = bytes[1];
		printf("Level %d Overhead: Map Data Ptr: %02x %02x\n", level->levelid, bytes[0], bytes[1]);
	}
	
}
