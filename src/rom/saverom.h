#include <stdlib.h>
#include <stdio.h>

#include "../main.h"

//	The defined values for various sizes really shouldn't be altered.  There are
//	very specific specs that need to be adhered to for NES ROMs.
#define SIZE_PRG_BANK 0x4000
#define SIZE_CHR_BANK 0x2000
#define SIZE_ROM_HEADER 16
#define SIZE_PALETTE 16

#define COUNT_PRG_BANK 8
#define COUNT_CHR_BANK 16

#define OFFSET_SPAWNS_TANK 0x1CA4B
#define OFFSET_SPAWNS_AFTERBOSS 0x1C5B2

#define OFFSET_BANK_0 0x0000
#define OFFSET_BANK_1 0x4000
#define OFFSET_BANK_2 0x8000
#define OFFSET_BANK_3 0xC000

#define OFFSET_PALETTE 0x0092 + SIZE_ROM_HEADER
#define OFFSET_SCROLLTABLE 0x00F2 + SIZE_ROM_HEADER
#define OFFSET_MAP 0x0152 + SIZE_ROM_HEADER
#define OFFSET_MAPDATA 0x1660 + SIZE_ROM_HEADER

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


unsigned short Out_LevelPointersOffset;
unsigned short Out_ROMLevelPointerAddr;


