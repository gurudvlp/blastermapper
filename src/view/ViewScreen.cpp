#include <array>
#include <cstdint>

#include <X11/X.h>
#include <X11/Xlib.h>
#include <GL/gl.h>
#include <GL/glx.h>
#include <GL/glu.h>

#include "ViewScreen.hpp"
#include "../level/Level.hpp"
#include "../level/LevelInfo.hpp"

namespace
{
using namespace blastmap::level;
void RenderScreen_Block(Level * level, BlockID blkid, int x, int y);
void RenderScreen_SubBlock(Level * level, SubBlockID sblkid, int x, int y);
void RenderScreen_UltraSubBlock(Level * level, UltraSubBlockID usblkid, int x, int y);
void RenderScreen_SpawnPoint(Level * level, int screen);
} // namespace

namespace blastmap {
using namespace level;
namespace view {

void ScreenRenderer::SetLevelMode(int lvl, int lvlmode)
{
    level_ = lvl;
    mode_ = lvlmode;
}

void ScreenRenderer::SetScreen(int screen)
{
    screen_ = screen;
}

void ScreenRenderer::SetSelection(int x, int y)
{
    selectedX_ = static_cast<unsigned char>(x);
    selectedY_ = static_cast<unsigned char>(y);
}

unsigned char ScreenRenderer::SelectedBlockX() const
{
    return selectedX_;
}

unsigned char ScreenRenderer::SelectedBlockY() const
{
    return selectedY_;
}

void ScreenRenderer::Render(unsigned char darken)
{
    Level * level = (Level *)&Levels[level_][mode_];

    glClearColor(1.0, 1.0, 1.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-1., 1., -1., 1., 1., 20.);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(0., 0., 10., 0., 0., 0., 0., 1., 0.);

    const int initx = (screen_ % 8) * 4;
    const int inity = (screen_ / 8) * 4;
    const int endx = initx + 4;
    const int endy = inity + 4;

    for(int y = inity; y < endy; y++)
    {
        for(int x = initx; x < endx; x++)
        {
            BlockID blkid = (*level).Map[x][y];
            int yat = y - inity;
            if(yat < 0) { yat += 256; }
            RenderScreen_Block(level, blkid, x - initx, 3 - yat);
        }
    }

    if(darken != 0x00)
    {
        for(int y = 0; y < 8; y++)
        {
            for(int x = 0; x < 8; x++)
            {
                if(y == selectedY_ && x == selectedX_)
                {
                    SetSelection(initx + x, inity + y);
                }
                else
                {
                    float sx = ((float)x * 2.0f) / 4.0f;
                    float sy = 4.0f - static_cast<float>(y);
                    sy = (sy * 2.0f) / 4.0f;

                    sx -= 1.0f;
                    sy -= 1.0f;

                    const float ex = sx + 0.5f;
                    const float ey = sy - 0.5f;

                    glBindTexture(GL_TEXTURE_2D, darkenTextureID);

                    glBegin(GL_QUADS);
                        glTexCoord2f(0, 0); glVertex3f(sx, ey, 0.1f);
                        glTexCoord2f(1, 0); glVertex3f(ex, ey, 0.1f);
                        glTexCoord2f(1, 1); glVertex3f(ex,  sy, 0.1f);
                        glTexCoord2f(0, 1); glVertex3f(sx,  sy, 0.1f);
                    glEnd();
                }
            }
        }
    }

    RenderScreen_SpawnPoint(level, screen_);
}

} // namespace view
} // namespace blastmap

namespace
{
void RenderScreen_Block(Level * level, BlockID blkid, int x, int y)
{
    SubBlockID subblocks[4];
    subblocks[0] = (*level).Blocks[blkid][0];
    subblocks[1] = (*level).Blocks[blkid][1];
    subblocks[2] = (*level).Blocks[blkid][2];
    subblocks[3] = (*level).Blocks[blkid][3];

    RenderScreen_SubBlock(level, subblocks[2], x * 2, y * 2);
    RenderScreen_SubBlock(level, subblocks[3], (x * 2) + 1, y * 2);
    RenderScreen_SubBlock(level, subblocks[0], x * 2, (y * 2) + 1);
    RenderScreen_SubBlock(level, subblocks[1], (x * 2) + 1, (y * 2) + 1);
}

void RenderScreen_SubBlock(Level * level, SubBlockID sblkid, int x, int y)
{
    UltraSubBlockID usbs[4];
    usbs[0] = (*level).SubBlocks[sblkid][0];
    usbs[1] = (*level).SubBlocks[sblkid][1];
    usbs[2] = (*level).SubBlocks[sblkid][2];
    usbs[3] = (*level).SubBlocks[sblkid][3];

    RenderScreen_UltraSubBlock(level, usbs[2], x * 2, y * 2);
    RenderScreen_UltraSubBlock(level, usbs[3], (x * 2) + 1, y * 2);
    RenderScreen_UltraSubBlock(level, usbs[0], x * 2, (y * 2) + 1);
    RenderScreen_UltraSubBlock(level, usbs[1], (x * 2) + 1, (y * 2) + 1);
}

void RenderScreen_UltraSubBlock(Level * level, UltraSubBlockID usblkid, int x, int y)
{
    float sx = (2.0f * static_cast<float>(x)) / 16.0f;
    float sy = (2.0f * static_cast<float>(y)) / 16.0f;

    sx -= 1.0f;
    sy -= 1.0f;

    glBindTexture(GL_TEXTURE_2D, (*level).USBTextures[usblkid].texid);

    glBegin(GL_QUADS);
        glTexCoord2f(0, 0); glVertex3f(sx, sy + (2.0f / 16.0f), 0.);
        glTexCoord2f(1, 0); glVertex3f(sx + (2.0f / 16.0f), sy + (2.0f / 16.0f), 0.);
        glTexCoord2f(1, 1); glVertex3f(sx + (2.0f / 16.0f),  sy, 0.);
        glTexCoord2f(0, 1); glVertex3f(sx,  sy, 0.);
    glEnd();
}

void RenderScreen_SpawnPoint(Level * level, int screen)
{
    unsigned short x = level->SpawnPoint.x;
    unsigned short y = level->SpawnPoint.y;

    int scx = screen % 8;
    int scy = screen / 8;

    auto drawSprite = [](GLuint texid, float sx, float sy, float ex, float ey, float depth)
    {
        glBindTexture(GL_TEXTURE_2D, texid);
        glBegin(GL_QUADS);
            glTexCoord2f(0, 0); glVertex3f(sx, ey, depth);
            glTexCoord2f(1, 0); glVertex3f(ex, ey, depth);
            glTexCoord2f(1, 1); glVertex3f(ex,  sy, depth);
            glTexCoord2f(0, 1); glVertex3f(sx,  sy, depth);
        glEnd();
    };

    auto computeCoords = [&](unsigned short valueX, unsigned short valueY) -> std::array<float, 4>
    {
        float sx = (2.0f * static_cast<float>(valueX)) / 16.0f;
        sx -= 1.0f;
        sx -= static_cast<float>(scx) * 2.0f;

        float sy = (2.0f * (31.0f - static_cast<float>(valueY))) / 16.0f;
        sy -= 3.0f;
        sy += static_cast<float>(scy) * 2.0f;

        float ex = sx + (2.0f / 16.0f);
        float ey = sy - (2.0f / 16.0f);

        return {sx, sy, ex, ey};
    };

    const auto baseCoords = computeCoords(x, y);
    drawSprite(spawnPointTextureID, baseCoords[0], baseCoords[1], baseCoords[2], baseCoords[3], 0.11f);

    for(int et = 0; et < 256; et++)
    {
        if(level->Things[et].thingtype == 0xFF) { break; }

        const auto thingCoords = computeCoords(level->Things[et].x, level->Things[et].y);
        drawSprite(thingSpawnTextureID, thingCoords[0], thingCoords[1], thingCoords[2], thingCoords[3], 0.11f);
    }
}
} // namespace
