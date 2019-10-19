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
	
	unsigned char * OutRom = build_InitializeRom();
	
	build_CopyProgramming(OutRom);
	build_SpawnPoints(OutRom);
	build_Palettes(OutRom);
	build_ScrollTables(OutRom);
	build_LevelDataPointers(OutRom);
	build_Maps(OutRom);


	
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
	
	fwrite((char *)&OutRom[0], SIZE_ROM_HEADER, 1, outfile);
	fwrite((char *)&OutRom[SIZE_ROM_HEADER], SIZE_PRG_BANK, 16, outfile);
	fclose(outfile);
	
	printf("Saved rebuilt ROM\n");
	
	return true;
}

bool Merge(char * sourcerom, char * destrom, short bank)
{
	//	Merge banks from a source ROM to a destination ROM.  This can be useful
	//	for using original banks in a custom ROM for debugging.  At least that's
	//	the idea.
	
	//	Copy PrgRom and ChrRom of source.  This is needed because when we open
	//	the ROM to save to, it will overwrite PrgRom and ChrRom in memory.
	printf("Copying source Prg and Chr banks...");
	unsigned char srcPrgRom[COUNT_PRG_BANK][SIZE_PRG_BANK];
	unsigned char srcChrRom[COUNT_CHR_BANK][SIZE_CHR_BANK];
	
	int ebank;
	for(ebank = 0; ebank < COUNT_PRG_BANK; ebank++)
	{
		memcpy(srcPrgRom[ebank], PrgRom[ebank], SIZE_PRG_BANK);
	}
	
	for(ebank = 0; ebank < COUNT_CHR_BANK; ebank++)
	{
		memcpy(srcChrRom[ebank], ChrRom[ebank], SIZE_CHR_BANK);
	}
	
	//	Load Destination rom and copy Prg and Chr
	printf("Loading %s...\n", destrom);
	LoadRom(destrom);
	
	//	In this iteration of code, you can only merge PRG banks.  So, we need
	//	to check which PRG bank to copy, copy it, and then save it to the
	//	destination file.
	printf("Roms checked, starting to merge...\n");
	memcpy(PrgRom[bank], srcPrgRom[bank], SIZE_PRG_BANK);
	
	//	That was pretty painless, time to save.
	FILE *outfile = fopen("blastermerge.nes", "w");
	
	fwrite((char *)&RomHeader[0], SIZE_ROM_HEADER, 1, outfile);
	fwrite((char *)&PrgRom[0], SIZE_PRG_BANK, COUNT_PRG_BANK, outfile);
	fwrite((char *)&ChrRom[0], SIZE_CHR_BANK, COUNT_CHR_BANK, outfile);
	fclose(outfile);
	
	printf("Merged bank %d and saved as 'blastermerge.nes'\n", bank);
	
	return true;
}

void OutRomAddressToBytes(unsigned short addr, unsigned char * bytes)
{
	bytes[0] = addr & 0xFF;
	bytes[1] = (addr >> 8) & 0xFF;
}
