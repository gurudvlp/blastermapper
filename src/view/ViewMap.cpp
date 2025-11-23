#include "ViewMap.hpp"

#include "../editor/EditorState.hpp"
#include "../level/Level.hpp"
#include "../level/LevelInfo.hpp"

#include <algorithm>

#include <GL/gl.h>
#include <GL/glu.h>

namespace
{
using namespace blastmap::level;

struct CameraState
{
    float centerX;
    float centerY;
    float halfWorld;
};

float clampCameraAxis(float center, float halfWorld)
{
    const float mapDimension = blastmap::editor::State::MapDimension;
    const float mapHalf = blastmap::editor::State::MapHalf;
    if(halfWorld >= mapHalf) { return mapHalf; }
    return std::clamp(center, halfWorld, mapDimension - halfWorld);
}

void RenderMap_Block(Level * level,
                     BlockID blkid,
                     int x,
                     int y,
                     const CameraState &camera);
void RenderMap_SubBlock(Level * level,
                        SubBlockID sblkid,
                        int x,
                        int y,
                        const CameraState &camera);
void RenderMap_UltraSubBlock(Level * level,
                             UltraSubBlockID usblkid,
                             int x,
                             int y,
                             const CameraState &camera);
void RenderMap_SpawnPoint(Level * level, const CameraState &camera);

} // namespace

namespace blastmap {
namespace view {

void MapRenderer::Render(const editor::State &state)
{
    Level * level = gLevelManager.level(state.level, state.mode);
    if(!level) { return; }

    glClearColor(1.0, 1.0, 1.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-1., 1., -1., 1., 1., 20.);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(0., 0., 10., 0., 0., 0., 0., 1., 0.);

    const float safeZoom = state.viewZoom > 0.0f ? state.viewZoom : 1.0f;
    const float halfWorld = editor::State::MapHalf / safeZoom;

    CameraState camera{
        clampCameraAxis(state.viewCenterX, halfWorld),
        clampCameraAxis(state.viewCenterY, halfWorld),
        halfWorld,
    };

    for(int y = 0; y < 32; ++y)
    {
        for(int x = 0; x < 32; ++x)
        {
            BlockID blkid = (*level).Map[x][y];
            RenderMap_Block(level, blkid, x, 31 - y, camera);
        }
    }

    RenderMap_SpawnPoint(level, camera);
}

} // namespace view
} // namespace blastmap

namespace
{
using namespace blastmap::level;

void RenderMap_Block(Level * level,
                     BlockID blkid,
                     int x,
                     int y,
                     const CameraState &camera)
{
    SubBlockID subblocks[4];
    subblocks[0] = (*level).Blocks[blkid][0];
    subblocks[1] = (*level).Blocks[blkid][1];
    subblocks[2] = (*level).Blocks[blkid][2];
    subblocks[3] = (*level).Blocks[blkid][3];

    RenderMap_SubBlock(level, subblocks[2], x * 2, y * 2, camera);
    RenderMap_SubBlock(level, subblocks[3], (x * 2) + 1, y * 2, camera);
    RenderMap_SubBlock(level, subblocks[0], x * 2, (y * 2) + 1, camera);
    RenderMap_SubBlock(level, subblocks[1], (x * 2) + 1, (y * 2) + 1, camera);
}

void RenderMap_SubBlock(Level * level,
                        SubBlockID sblkid,
                        int x,
                        int y,
                        const CameraState &camera)
{
    UltraSubBlockID usbs[4];
    usbs[0] = (*level).SubBlocks[sblkid][0];
    usbs[1] = (*level).SubBlocks[sblkid][1];
    usbs[2] = (*level).SubBlocks[sblkid][2];
    usbs[3] = (*level).SubBlocks[sblkid][3];

    RenderMap_UltraSubBlock(level, usbs[2], x * 2, y * 2, camera);
    RenderMap_UltraSubBlock(level, usbs[3], (x * 2) + 1, y * 2, camera);
    RenderMap_UltraSubBlock(level, usbs[0], x * 2, (y * 2) + 1, camera);
    RenderMap_UltraSubBlock(level, usbs[1], (x * 2) + 1, (y * 2) + 1, camera);
}

void RenderMap_UltraSubBlock(Level * level,
                             UltraSubBlockID usblkid,
                             int x,
                             int y,
                             const CameraState &camera)
{
    float left = (static_cast<float>(x) - camera.centerX) / camera.halfWorld;
    float right = (static_cast<float>(x + 1) - camera.centerX) / camera.halfWorld;
    float bottom = (static_cast<float>(y) - camera.centerY) / camera.halfWorld;
    float top = (static_cast<float>(y + 1) - camera.centerY) / camera.halfWorld;

    glBindTexture(GL_TEXTURE_2D, (*level).USBTextures[usblkid].texid);

    glBegin(GL_QUADS);
        glTexCoord2f(0, 0); glVertex3f(left, top, 0.);
        glTexCoord2f(1, 0); glVertex3f(right, top, 0.);
        glTexCoord2f(1, 1); glVertex3f(right, bottom, 0.);
        glTexCoord2f(0, 1); glVertex3f(left, bottom, 0.);
    glEnd();
}

void RenderMap_SpawnPoint(Level * level, const CameraState &camera)
{
    float worldX = static_cast<float>((*level).SpawnPoint.x);
    float worldY = static_cast<float>((*level).SpawnPoint.y);

    float left = (worldX - camera.centerX) / camera.halfWorld;
    float right = ((worldX + 1.0f) - camera.centerX) / camera.halfWorld;
    float bottom = (worldY - camera.centerY) / camera.halfWorld;
    float top = ((worldY + 1.0f) - camera.centerY) / camera.halfWorld;

    glBindTexture(GL_TEXTURE_2D, gLevelManager.spawnPointTextureID());

    glBegin(GL_QUADS);
        glTexCoord2f(0, 0); glVertex3f(left, top, 0.11f);
        glTexCoord2f(1, 0); glVertex3f(right, top, 0.11f);
        glTexCoord2f(1, 1); glVertex3f(right, bottom, 0.11f);
        glTexCoord2f(0, 1); glVertex3f(left, bottom, 0.11f);
    glEnd();
}

} // namespace
