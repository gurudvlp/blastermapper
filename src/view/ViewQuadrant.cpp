#include <cstdint>
#include <array>

#include <GL/gl.h>
#include <GL/glu.h>

#include "ViewQuadrant.hpp"
#include "../level/Level.hpp"
#include "../level/LevelInfo.hpp"
namespace
{
using namespace blastmap::level;
void RenderQuadrant_Block(Level * level, BlockID blkid, int x, int y);
void RenderQuadrant_SubBlock(Level * level, SubBlockID sblkid, int x, int y);
void RenderQuadrant_UltraSubBlock(Level * level, UltraSubBlockID usblkid, int x, int y);
void RenderQuadrant_SpawnPoint(Level * level, int quadrant);
} // namespace

namespace blastmap {
using namespace level;
namespace view {

void QuadrantRenderer::SetLevelMode(int lvl, int lvlmode)
{
    level_ = lvl;
    mode_ = lvlmode;
}

void QuadrantRenderer::SetQuadrant(int quadrant)
{
    quadrant_ = quadrant;
}

void QuadrantRenderer::SetSelection(int x, int y)
{
    selectedX_ = x;
    selectedY_ = y;
}

void QuadrantRenderer::Render(unsigned char darken)
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

	int initx = 0;
	int inity = 0;
	int endx = 0;
	int endy = 0;

	if(quadrant_ == 0)
	{
		endx = 16;
		endy = 16;
	}
	else if(quadrant_ == 1)
	{
		initx = 16;
		endx = 32;
		endy = 16;
	}
	else if(quadrant_ == 2)
	{
		inity = 16;
		endx = 16;
		endy = 32;
	}
	else
	{
		initx = 16;
		inity = 16;
		endx = 32;
		endy = 32;
	}

	for(int y = inity; y < endy; y++)
	{
		for(int x = initx; x < endx; x++)
		{
			BlockID blkid = (*level).Map[x][y];
			int yat = y - inity;
			if(yat < 0) { yat += 64; }
			RenderQuadrant_Block(level, blkid, x - initx, 15 - yat);
		}
	}

	if(darken != 0x00)
	{
		for(int y = 0; y < 4; y++)
		{
			for(int x = 0; x < 4; x++)
			{
				if(y == selectedY_ && x == selectedX_)
				{
					// selected screen; nothing to do
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

	RenderQuadrant_SpawnPoint(level, quadrant_);
}
} // namespace view
} // namespace blastmap

namespace
{
void RenderQuadrant_Block(Level * level, BlockID blkid, int x, int y)
{
	SubBlockID subblocks[4];
	subblocks[0] = (*level).Blocks[blkid][0];
	subblocks[1] = (*level).Blocks[blkid][1];
	subblocks[2] = (*level).Blocks[blkid][2];
	subblocks[3] = (*level).Blocks[blkid][3];

	RenderQuadrant_SubBlock(level, subblocks[2], x * 2, y * 2);
	RenderQuadrant_SubBlock(level, subblocks[3], (x * 2) + 1, y * 2);
	RenderQuadrant_SubBlock(level, subblocks[0], x * 2, (y * 2) + 1);
	RenderQuadrant_SubBlock(level, subblocks[1], (x * 2) + 1, (y * 2) + 1);
}

void RenderQuadrant_SubBlock(Level * level, SubBlockID sblkid, int x, int y)
{
	UltraSubBlockID usbs[4];
	usbs[0] = (*level).SubBlocks[sblkid][0];
	usbs[1] = (*level).SubBlocks[sblkid][1];
	usbs[2] = (*level).SubBlocks[sblkid][2];
	usbs[3] = (*level).SubBlocks[sblkid][3];

	RenderQuadrant_UltraSubBlock(level, usbs[2], x * 2, y * 2);
	RenderQuadrant_UltraSubBlock(level, usbs[3], (x * 2) + 1, y * 2);
	RenderQuadrant_UltraSubBlock(level, usbs[0], x * 2, (y * 2) + 1);
	RenderQuadrant_UltraSubBlock(level, usbs[1], (x * 2) + 1, (y * 2) + 1);
}

void RenderQuadrant_UltraSubBlock(Level * level, UltraSubBlockID usblkid, int x, int y)
{
	float sx = (2.0f * static_cast<float>(x)) / 64.0f;
	float sy = (2.0f * static_cast<float>(y)) / 64.0f;

	sx -= 1.0f;
	sy -= 1.0f;

	glBindTexture(GL_TEXTURE_2D, (*level).USBTextures[usblkid].texid);

	glBegin(GL_QUADS);
		glTexCoord2f(0, 0); glVertex3f(sx, sy + (2.0f / 64.0f), 0.);
		glTexCoord2f(1, 0); glVertex3f(sx + (2.0f / 64.0f), sy + (2.0f / 64.0f), 0.);
		glTexCoord2f(1, 1); glVertex3f(sx + (2.0f / 64.0f),  sy, 0.);
		glTexCoord2f(0, 1); glVertex3f(sx,  sy, 0.);
	glEnd();
}

void RenderQuadrant_SpawnPoint(Level * level, int quadrant)
{
	const auto drawSprite = [](GLuint texid, float sx, float sy, float ex, float ey) {
		glBindTexture(GL_TEXTURE_2D, texid);
		glBegin(GL_QUADS);
			glTexCoord2f(0, 0); glVertex3f(sx, ey, 0.11f);
			glTexCoord2f(1, 0); glVertex3f(ex, ey, 0.11f);
			glTexCoord2f(1, 1); glVertex3f(ex,  sy, 0.11f);
			glTexCoord2f(0, 1); glVertex3f(sx,  sy, 0.11f);
		glEnd();
	};

	auto computeCoords = [&](unsigned short valueX, unsigned short valueY) -> std::array<float, 4> {
		float sx = (2.0f * static_cast<float>(valueX)) / 64.0f;
		sx -= 1.0f;
		if(quadrant == 1 || quadrant == 3) { sx -= 2.0f; }

		float sy = (2.0f * (127.0f - static_cast<float>(valueY))) / 64.0f;
		sy -= 3.0f;
		if(quadrant == 2 || quadrant == 3) { sy += 2.0f; }

		float ex = sx + (2.0f / 64.0f);
		float ey = sy + (2.0f / 64.0f);

		return {sx, sy, ex, ey};
	};

	const auto spawnCoords = computeCoords(level->SpawnPoint.x, level->SpawnPoint.y);
		drawSprite(gLevelManager.spawnPointTextureID(), spawnCoords[0], spawnCoords[1], spawnCoords[2], spawnCoords[3]);

	for(int et = 0; et < 256; et++)
	{
		if(level->Things[et].thingtype == 0xFF) { break; }

		const auto thingCoords = computeCoords(level->Things[et].x, level->Things[et].y);
		drawSprite(gLevelManager.thingSpawnTextureID(), thingCoords[0], thingCoords[1], thingCoords[2], thingCoords[3]);
	}
}
} // namespace
