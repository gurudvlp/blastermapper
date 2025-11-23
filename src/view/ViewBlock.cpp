#include <cstdint>

#include <X11/X.h>
#include <X11/Xlib.h>
#include <GL/gl.h>
#include <GL/glx.h>
#include <GL/glu.h>

#include "ViewBlock.hpp"
#include "../level/leveltypes.h"

namespace
{
void RenderBlock_Block(Level * level, BlockID blkid, int x, int y);
void RenderBlock_SubBlock(Level * level, SubBlockID sblkid, int x, int y);
void RenderBlock_UltraSubBlock(Level * level, UltraSubBlockID usblkid, int x, int y);
} // namespace

namespace blastmap {
namespace view {

void BlockRenderer::SetLevelMode(int lvl, int lvlmode)
{
    level_ = lvl;
    mode_ = lvlmode;
}

void BlockRenderer::SetBlock(int block)
{
    block_ = block;
}

void BlockRenderer::Render()
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

        const int initx = block_ / 32;
        const int inity = block_ % 32;
	const int endx = initx + 32;
	const int endy = inity + 32;

	for(int y = inity; y < endy; y++)
	{
        for(int x = initx; x < endx; x++)
        {
            BlockID blkid = (*level).Map[x][y];
            RenderBlock_Block(level, blkid, 0, 0);
        }
	}
}

} // namespace view
} // namespace blastmap

namespace
{
void RenderBlock_Block(Level * level, BlockID blkid, int x, int y)
{
	SubBlockID subblocks[4];
	subblocks[0] = (*level).Blocks[blkid][0];
	subblocks[1] = (*level).Blocks[blkid][1];
	subblocks[2] = (*level).Blocks[blkid][2];
	subblocks[3] = (*level).Blocks[blkid][3];

	RenderBlock_SubBlock(level, subblocks[0], x * 2, y * 2);
	RenderBlock_SubBlock(level, subblocks[1], (x * 2) + 1, y * 2);
	RenderBlock_SubBlock(level, subblocks[2], x * 2, (y * 2) + 1);
	RenderBlock_SubBlock(level, subblocks[3], (x * 2) + 1, (y * 2) + 1);
}

void RenderBlock_SubBlock(Level * level, SubBlockID sblkid, int x, int y)
{
	UltraSubBlockID usbs[4];
	usbs[0] = (*level).SubBlocks[sblkid][0];
	usbs[1] = (*level).SubBlocks[sblkid][1];
	usbs[2] = (*level).SubBlocks[sblkid][2];
	usbs[3] = (*level).SubBlocks[sblkid][3];

	RenderBlock_UltraSubBlock(level, usbs[0], x * 2, y * 2);
	RenderBlock_UltraSubBlock(level, usbs[1], (x * 2) + 1, y * 2);
	RenderBlock_UltraSubBlock(level, usbs[2], x * 2, (y * 2) + 1);
	RenderBlock_UltraSubBlock(level, usbs[3], (x * 2) + 1, (y * 2) + 1);
}

void RenderBlock_UltraSubBlock(Level * level, UltraSubBlockID usblkid, int x, int y)
{
	float sx = (2.0f * static_cast<float>(x)) / 4.0f;
	float sy = (2.0f * static_cast<float>(y)) / 4.0f;

	sx -= 1.0f;
	sy -= 1.0f;

	glBindTexture(GL_TEXTURE_2D, (*level).USBTextures[usblkid].texid);

	glBegin(GL_QUADS);
		glTexCoord2f(0, 0); glVertex3f(sx, sy + (2.0f / 4.0f), 0.);
		glTexCoord2f(1, 0); glVertex3f(sx + (2.0f / 4.0f), sy + (2.0f / 4.0f), 0.);
		glTexCoord2f(1, 1); glVertex3f(sx + (2.0f / 4.0f),  sy, 0.);
		glTexCoord2f(0, 1); glVertex3f(sx,  sy, 0.);
	glEnd();
}
} // namespace
