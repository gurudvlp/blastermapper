#include <cstdlib>
#include <cstdio>
#include <cstring>

#include <GL/gl.h>
#include <GL/glu.h>

#include "LevelLoader.hpp"
#include "../view/Palette.hpp"
#include "../level/LevelInfo.hpp"

namespace blastmap {
using namespace level;
namespace rom {

LevelLoader::LevelLoader(RomState & state)
    : state_(state)
{
}

bool LevelLoader::PopulateLevels()
{
    InitializeLevels();

    if(!FindLevelPointers())
    {
        return false;
    }

    for(int el = 0; el < 8; ++el)
    {
        Level * tank = gLevelManager.level(el, 0);
        if(!tank || !LoadLevel(tank))
        {
            std::printf("Failed to load level %d (tank)\n", el);
            return false;
        }

        Level * overhead = gLevelManager.level(el, 1);
        if(!overhead || !LoadLevel(overhead))
        {
            std::printf("Failed to load level %d (overhead)\n", el);
            return false;
        }
    }

    return true;
}

void LevelLoader::InitializeLevels()
{
    for(int el = 0; el < static_cast<int>(LevelCount); ++el)
    {
        Level * tank = gLevelManager.level(el, 0);
        Level * overhead = gLevelManager.level(el, 1);
        if(!tank || !overhead) { continue; }

        *tank = Level{};
        *overhead = Level{};

        tank->levelid = el;
        overhead->levelid = el;
        tank->leveltype = LevelType::Tank;
        overhead->leveltype = LevelType::Overhead;
    }
}

bool LevelLoader::FindLevelPointers()
{
    auto & PrgRom = state_.PrgRom();

    int el;
    unsigned short addr;

    for(el = 0; el < 5; el++)
    {
        Level * tank = gLevelManager.level(el, 0);
        if(!tank) { continue; }

        addr = PrgRom[0][(el * 4) + 1] << 8;
        addr += PrgRom[0][(el * 4)];
        addr -= 0x8000;
        tank->romlevelpointer = addr;

        addr = PrgRom[0][(el * 4) + 3] << 8;
        addr += PrgRom[0][(el * 4) + 2];
        addr -= 0x8000;
        tank->datapointers.scrolltable = addr;
    }

    for(el = 0; el < 3; el++)
    {
        Level * tank = gLevelManager.level(el + 5, 0);
        if(!tank) { continue; }

        addr = PrgRom[1][(el * 4) + 1] << 8;
        addr += PrgRom[1][(el * 4)];
        addr -= 0x8000;
        tank->romlevelpointer = addr;

        addr = PrgRom[1][(el * 4) + 3] << 8;
        addr += PrgRom[1][(el * 4) + 2];
        addr -= 0x8000;
        tank->datapointers.scrolltable = addr;
    }

    Level * overhead0 = gLevelManager.level(0, 1);
    if(overhead0)
    {
        addr = PrgRom[1][13] << 8;
        addr += PrgRom[1][12];
        addr -= 0x8000;
        overhead0->romlevelpointer = addr;

        addr = PrgRom[1][15] << 8;
        addr += PrgRom[1][14];
        addr -= 0x8000;
        overhead0->datapointers.scrolltable = addr;
    }

    Level * overhead2 = gLevelManager.level(2, 1);
    if(overhead2)
    {
        addr = PrgRom[1][17] << 8;
        addr += PrgRom[1][16];
        addr -= 0x8000;
        overhead2->romlevelpointer = addr;

        addr = PrgRom[1][19] << 8;
        addr += PrgRom[1][18];
        addr -= 0x8000;
        overhead2->datapointers.scrolltable = addr;
    }

    auto storeOverhead = [&](int lvl,
                             unsigned short highLow,
                             unsigned short highHigh,
                             unsigned short scrollLow,
                             unsigned short scrollHigh)
    {
        Level * overhead = gLevelManager.level(lvl, 1);
        if(!overhead) { return; }

        unsigned short romAddr = PrgRom[2][highHigh] << 8;
        romAddr += PrgRom[2][highLow];
        romAddr -= 0x8000;
        overhead->romlevelpointer = romAddr;

        unsigned short scrollAddr = PrgRom[2][scrollHigh] << 8;
        scrollAddr += PrgRom[2][scrollLow];
        scrollAddr -= 0x8000;
        overhead->datapointers.scrolltable = scrollAddr;
    };

    storeOverhead(1, 4, 5, 6, 7);
    storeOverhead(3, 16, 17, 18, 19);
    storeOverhead(4, 0, 1, 2, 3);
    storeOverhead(5, 8, 9, 10, 11);
    storeOverhead(6, 20, 21, 22, 23);
    storeOverhead(7, 12, 13, 14, 15);

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

        Level * target = nullptr;
        int lvlIndex = (el < 8) ? el : (el - 8);
        int lvlmode = (el < 8) ? 1 : 0;
        target = gLevelManager.level(lvlIndex, lvlmode);
        if(!target) { continue; }

        target->datapointers.thinglist.listpointer = addr;
        target->datapointers.thinglist.typelistpointer = typeptr;
        target->datapointers.thinglist.xlistpointer = xptr;
        target->datapointers.thinglist.ylistpointer = yptr;
    }

    return true;
}

bool LevelLoader::LoadLevel(Level * level)
{
    auto & PrgRom = state_.PrgRom();

    int bank = 0;
    if((*level).leveltype == LevelType::Tank)
    {
        if((*level).levelid < 5) { bank = 0; }
        else { bank = 1; }
    }
    else
    {
        if((*level).levelid == 0 || ((*level).levelid == 2)) { bank = 1; }
        else { bank = 2; }
    }

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

    for(int esp = 0; esp < 4; esp++)
    {
        (*level).SubPalettes[esp].color0 = PrgRom[bank][(*level).datapointers.palette + (esp * 4)];
        (*level).SubPalettes[esp].color1 = PrgRom[bank][(*level).datapointers.palette + (esp * 4) + 1];
        (*level).SubPalettes[esp].color2 = PrgRom[bank][(*level).datapointers.palette + (esp * 4) + 2];
        (*level).SubPalettes[esp].color3 = PrgRom[bank][(*level).datapointers.palette + (esp * 4) + 3];
    }

    int x, y;
    unsigned short highblockid = 0;
    unsigned short basemapaddr = (*level).datapointers.map;

    for(y = 0; y < 32; y++)
    {
        for(x = 0; x < 32; x++)
        {
            (*level).Map[x][y] = (BlockID)PrgRom[bank][basemapaddr + (y * 32) + x];
            if((*level).Map[x][y] > highblockid) { highblockid = (*level).Map[x][y]; }
        }
    }

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

    unsigned short baseusbaddr = (*level).datapointers.usbtable;

    for(x = 0; x <= highusbid; x++)
    {
        (*level).UltraSubBlocks[x][0] = (ChrID)PrgRom[bank][baseusbaddr + (x * 4)];
        (*level).UltraSubBlocks[x][1] = (ChrID)PrgRom[bank][baseusbaddr + (x * 4) + 1];
        (*level).UltraSubBlocks[x][2] = (ChrID)PrgRom[bank][baseusbaddr + (x * 4) + 2];
        (*level).UltraSubBlocks[x][3] = (ChrID)PrgRom[bank][baseusbaddr + (x * 4) + 3];
    }

    unsigned short baseusba = (*level).datapointers.usbattribute;

    for(x = 0; x <= highusbid; x++)
    {
        unsigned char tbyte = PrgRom[bank][baseusba + x];

        if((*level).leveltype == LevelType::Overhead)
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

    unsigned short basestaddr = (*level).datapointers.scrolltable;

    for(x = 0; x < 16; x++)
    {
        (*level).ScrollTable[x] = PrgRom[bank][basestaddr + x];
    }

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

    FindSpawnPoint(level);

    return true;
}

void LevelLoader::LoadUSBTextures(int levelIndex, int lvlmode)
{
    Level * target = gLevelManager.level(levelIndex, lvlmode);
    if(!target) { return; }
    CreateUSBTextures(target);
}

void LevelLoader::CreateUSBTextures(Level * level)
{
    unsigned short offset = 0x00;
    int bank = 0;
    auto & ChrRom = state_.ChrRom();

    if((*level).leveltype == LevelType::Tank)
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

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    int eusb;
    for(eusb = 0; eusb < 256; eusb++)
    {
        glGenTextures(1, &(*level).USBTextures[eusb].texid);
        glBindTexture(GL_TEXTURE_2D, (*level).USBTextures[eusb].texid);

        unsigned char upal[4];
        unsigned char subpal = (*level).USBAttributeTable[eusb].subpalette;
        upal[0] = (*level).SubPalettes[subpal].color0;
        upal[1] = (*level).SubPalettes[subpal].color1;
        upal[2] = (*level).SubPalettes[subpal].color2;
        upal[3] = (*level).SubPalettes[subpal].color3;

        for(y = 0; y < 8; y++)
        {
            for(x = 0; x < 8; x++)
            {
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

        for(y = 0; y < 8; y++)
        {
            for(x = 0; x < 8; x++)
            {
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

        for(y = 0; y < 8; y++)
        {
            for(x = 0; x < 8; x++)
            {
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

        for(y = 0; y < 8; y++)
        {
            for(x = 0; x < 8; x++)
            {
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
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 16, 16, 0, GL_RGBA, GL_UNSIGNED_BYTE,
                     (*level).USBTextures[eusb].data.data());
    }
}

void LevelLoader::FindSpawnPoint(Level * level)
{
    unsigned short addr = 0x0000;
    unsigned short bank = 7;
    auto & PrgRom = state_.PrgRom();

    if(level->leveltype == LevelType::Overhead) { addr = 0x05A2; }
    else { addr = 0x0A3B; }

    addr += level->levelid * 2;

    level->SpawnPoint.x = PrgRom[bank][addr];
    level->SpawnPoint.y = PrgRom[bank][addr + 1];
}

} // namespace rom
} // namespace blastmap
