#include <array>
#include <cstdint>
#include <cstdio>

#include <GL/gl.h>
#include <GL/glu.h>

#include "ViewMap.hpp"
#include "Palette.hpp"
#include "../level/Level.hpp"
#include "../level/LevelInfo.hpp"

namespace
{
using namespace blastmap::level;
void RenderMap_Block(Level * level, BlockID blkid, int x, int y, unsigned char darken);
void RenderMap_SubBlock(Level * level, SubBlockID sblkid, int x, int y);
void RenderMap_UltraSubBlock(Level * level, UltraSubBlockID usblkid, int x, int y);
void RenderMap_SpawnPoint(Level * level);
void DarkenAllQuandrantsBut(int selectedx, int selectedy);
void QuadScreenCoords(int x, int y, float * sx, float * sy, float * ex, float * ey);
} // namespace

namespace blastmap {
using namespace level;
namespace view {

void MapRenderer::SetLevelMode(int lvl, int lvlmode)
{
	level_ = lvl;
	mode_ = lvlmode;
}

void MapRenderer::SetSelection(int x, int y)
{
	selectedX_ = x;
	selectedY_ = y;
}

	void MapRenderer::Render(unsigned char darken)
	{
	Level * level = gLevelManager.level(level_, mode_);
	if(!level) { return; }

	glClearColor(1.0, 1.0, 1.0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(-1., 1., -1., 1., 1., 20.);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(0., 0., 10., 0., 0., 0., 0., 1., 0.);

	for(int y = 0; y < 32; y++)
	{
		for(int x = 0; x < 32; x++)
		{
			BlockID blkid = (*level).Map[x][y];

				if(darken == 0x00 || (x == selectedX_ && y == selectedY_)) { RenderMap_Block(level, blkid, x, 31 - y, 0x00); }
				else { RenderMap_Block(level, blkid, x, 31 - y, 0x01); }
		}
	}

	if(darken != 0x00) { DarkenAllQuandrantsBut(selectedX_, selectedY_); }
	RenderMap_SpawnPoint(level);
}
} // namespace view
} // namespace blastmap

namespace
{
void RenderMap_Block(Level * level, BlockID blkid, int x, int y, unsigned char darken)
{
	SubBlockID subblocks[4];
	subblocks[0] = (*level).Blocks[blkid][0];
	subblocks[1] = (*level).Blocks[blkid][1];
	subblocks[2] = (*level).Blocks[blkid][2];
	subblocks[3] = (*level).Blocks[blkid][3];

	RenderMap_SubBlock(level, subblocks[2], x * 2, y * 2);
	RenderMap_SubBlock(level, subblocks[3], (x * 2) + 1, y * 2);
	RenderMap_SubBlock(level, subblocks[0], x * 2, (y * 2) + 1);
	RenderMap_SubBlock(level, subblocks[1], (x * 2) + 1, (y * 2) + 1);
}

void RenderMap_SubBlock(Level * level, SubBlockID sblkid, int x, int y)
{
	UltraSubBlockID usbs[4];
	usbs[0] = (*level).SubBlocks[sblkid][0];
	usbs[1] = (*level).SubBlocks[sblkid][1];
	usbs[2] = (*level).SubBlocks[sblkid][2];
	usbs[3] = (*level).SubBlocks[sblkid][3];

	RenderMap_UltraSubBlock(level, usbs[2], x * 2, y * 2);
	RenderMap_UltraSubBlock(level, usbs[3], (x * 2) + 1, y * 2);
	RenderMap_UltraSubBlock(level, usbs[0], x * 2, (y * 2) + 1);
	RenderMap_UltraSubBlock(level, usbs[1], (x * 2) + 1, (y * 2) + 1);
}

void RenderMap_UltraSubBlock(Level * level, UltraSubBlockID usblkid, int x, int y)
{
	float sx, sy;

	sx = (2.0f * static_cast<float>(x)) / 128.0f;
	sy = (2.0f * static_cast<float>(y)) / 128.0f;

	sx -= 1.0f;
	sy -= 1.0f;

	glBindTexture(GL_TEXTURE_2D, (*level).USBTextures[usblkid].texid);

	glBegin(GL_QUADS);

		glTexCoord2f(0, 0); glVertex3f(sx, sy + (2.0f / 128.0f), 0.);
		glTexCoord2f(1, 0); glVertex3f(sx + (2.0f / 128.0f), sy + (2.0f / 128.0f), 0.);
		glTexCoord2f(1, 1); glVertex3f(sx + (2.0f / 128.0f),  sy, 0.);
		glTexCoord2f(0, 1); glVertex3f(sx,  sy, 0.);
	glEnd();
}

void DarkenAllQuandrantsBut(int selectedx, int selectedy)
{
	for(int y = 0; y < 2; y++)
	{
		for(int x = 0; x < 2; x++)
		{
			if(x != selectedx || y != selectedy)
			{
				float sx, sy, ex, ey;
				QuadScreenCoords(x, y, &sx, &sy, &ex, &ey);

			glBindTexture(GL_TEXTURE_2D, gLevelManager.darkenTextureID());

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

void QuadScreenCoords(int x, int y, float * sx, float * sy, float * ex, float * ey)
{
	*sx = static_cast<float>(x);
	*sy = static_cast<float>(y);

	*sx = (*sx * 2.0f) / 2.0f;
	*sy = (*sy * 2.0f) / 2.0f;

	*sx -= 1.0f;
	*sy -= 1.0f;

	*ex = *sx + 1.0f;
	*ey = *sy + 1.0f;
}

void RenderMap_SpawnPoint(Level * level)
{
	float sx, sy, ex, ey;
	sx = (2.0f * static_cast<float>((*level).SpawnPoint.x)) / 128.0f;
	sx -= 1.0f;
	sy = (2.0f * static_cast<float>((*level).SpawnPoint.y)) / 128.0f;
	sy -= 1.0f;

	ex = sx + (2.0f / 128.0f);
	ey = sy + (2.0f / 128.0f);

	glBindTexture(GL_TEXTURE_2D, gLevelManager.spawnPointTextureID());

	glBegin(GL_QUADS);

		glTexCoord2f(0, 0); glVertex3f(sx, ey, 0.11f);
		glTexCoord2f(1, 0); glVertex3f(ex, ey, 0.11f);
		glTexCoord2f(1, 1); glVertex3f(ex,  sy, 0.11f);
		glTexCoord2f(0, 1); glVertex3f(sx,  sy, 0.11f);
	glEnd();
}
} // namespace
