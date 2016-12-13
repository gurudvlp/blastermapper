#include <stdlib.h>
#include <stdio.h>

#include "main.h"
//#include "leveltypes.h"

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


