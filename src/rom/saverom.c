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
	
	//	Copy PrgRom and ChrRom of source
	
	
	//	Load Destination rom and copy Prg and Chr
	printf("Loading %s...\n", destrom);
	LoadRom(destrom);
	
	printf("Roms checked, starting to merge...\n");
	printf("Function not fully implemented.  Sorry.\n");
	
	return true;
}

void OutRomAddressToBytes(unsigned short addr, unsigned char * bytes)
{
	bytes[0] = addr & 0xFF;
	bytes[1] = (addr >> 8) & 0xFF;
}
