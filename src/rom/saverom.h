#include <stdlib.h>
#include <stdio.h>

#include "../main.h"
//#include "leveltypes.h"

//	The defined values for various sizes really shouldn't be altered.  There are
//	very specific specs that need to be adhered to for NES ROMs.
#define SIZE_PRG_BANK 0x4000
#define SIZE_CHR_BANK 0x2000
#define SIZE_ROM_HEADER 16

#define COUNT_PRG_BANK 8
#define COUNT_CHR_BANK 16

#define OFFSET_SPAWNS_TANK 0x1CA4B
#define OFFSET_SPAWNS_AFTERBOSS 0x1C5B2

typedef struct
{
	unsigned short BlockSize;
	unsigned short SubBlockSize;
	unsigned short USBSize;
	unsigned short USBAttrSize;
} SerializedMapInfo;

bool SaveRom();
void OutRomAddressToBytes(unsigned short addr, unsigned char * bytes);
unsigned char MapToBytes(unsigned char * mapdata, Level * level);
//unsigned short BuildMapData(unsigned char * mapmeta, Level * level,  unsigned char highblock, unsigned short * blocksize, unsigned short * subblocksize, unsigned short * usbsize, unsigned short * usbattrsize);
unsigned short BuildMapData(unsigned char * mapmeta, Level * level, unsigned char highblock, SerializedMapInfo * smi);
unsigned char USBAttributeToByte(USBAttributes * usbattr);
void BuildSubPalettes(Level * level, unsigned char * spbytes);
void BuildMapPointers(Level * level, unsigned short startloc, SerializedMapInfo * smi, unsigned char * outbuf);

unsigned short Out_PaletteOffset;
unsigned short Out_MapOffset;
unsigned short Out_OvMapOffset;
unsigned short Out_MapDataOffset;
unsigned short Out_OvMapDataOffset;
unsigned short Out_LevelPointersOffset;
unsigned short Out_ROMLevelPointerAddr;


