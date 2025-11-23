#include <cstdio>
#include <cstdlib>
#include <cstring>

#include <SDL2/SDL.h>
#include <GL/gl.h>
#include <GL/glu.h>

#include "main.hpp"
#include "editor/EditorState.hpp"
#include "platform/Window.hpp"
#include "rom/Rom.hpp"
#include "view/EditorRenderer.hpp"
#include "view/Palette.hpp"
#include "view/ViewBlock.hpp"
#include "view/ViewMap.hpp"
#include "view/ViewQuadrant.hpp"
#include "view/ViewScreen.hpp"
#include "level/LevelInfo.hpp"

extern "C" {
bool SAVEROM_SHOW_LEVEL_POINTERS = false;

bool IsCmdOptionSet(int argc, char ** argv, const char option[])
{
    for(int ea = 0; ea < argc; ++ea)
    {
        if(strcmp(argv[ea], option) == 0) { return true; }
    }
    return false;
}
}

namespace blastmap {
using namespace level;

namespace {
rom::Rom gRom;
}
void GetCmdEditorCoords(int argc, char ** argv, editor::State &state)
{
    for(int x = 0; x < 128; ++x)
    {
        char instr[16] = {};
        std::snprintf(instr, sizeof(instr), "x%d", x);
        if(IsCmdOptionSet(argc, argv, instr)) { state.x = x; }

        std::snprintf(instr, sizeof(instr), "y%d", x);
        if(IsCmdOptionSet(argc, argv, instr)) { state.y = x; }
    }
}

void ApplyCommandLineOptions(editor::State &state, int argc, char **argv)
{
    for(int lvl = 0; lvl < 8; ++lvl)
    {
        for(int mode = 0; mode < 2; ++mode)
        {
            char levelOpt[16] = {};
            std::snprintf(levelOpt, sizeof(levelOpt), "level%d%d", lvl + 1, mode);
            if(IsCmdOptionSet(argc, argv, levelOpt))
            {
                state.level = lvl;
                state.mode = static_cast<unsigned char>(mode);
            }
        }
    }

    for(int quad = 0; quad < 4; ++quad)
    {
        char quadOpt[8];
        std::snprintf(quadOpt, sizeof(quadOpt), "quad%d", quad);
        if(IsCmdOptionSet(argc, argv, quadOpt)) { state.quadrant = quad; }
    }

    if(IsCmdOptionSet(argc, argv, "zoommap")) { state.zoom = editor::ZoomMode::Map; }
    if(IsCmdOptionSet(argc, argv, "zoomquadrant")) { state.zoom = editor::ZoomMode::Quadrant; }
    if(IsCmdOptionSet(argc, argv, "zoomscreen")) { state.zoom = editor::ZoomMode::Screen; }
    if(IsCmdOptionSet(argc, argv, "zoomblock")) { state.zoom = editor::ZoomMode::Block; }

    GetCmdEditorCoords(argc, argv, state);

    if(IsCmdOptionSet(argc, argv, "levelpointers")) { SAVEROM_SHOW_LEVEL_POINTERS = true; }
    else { SAVEROM_SHOW_LEVEL_POINTERS = false; }
}

bool HandleKeyPress(editor::State &editor, SDL_Keycode key, const view::EditorRenderer &renderer)
{
    const unsigned char currentSelectedX = renderer.SelectedBlockX();
    const unsigned char currentSelectedY = renderer.SelectedBlockY();

    if(key == SDLK_ESCAPE)
    {
        if(editor.zoom == editor::ZoomMode::Map) { return false; }
        editor.zoomOut();
        return true;
    }

    if(key == SDLK_UP) { editor.moveUp(); return true; }
    if(key == SDLK_DOWN) { editor.moveDown(); return true; }
    if(key == SDLK_LEFT) { editor.moveLeft(); return true; }
    if(key == SDLK_RIGHT) { editor.moveRight(); return true; }

    if(key == SDLK_RETURN || key == SDLK_KP_ENTER) { editor.zoomIn(); return true; }
    if(key == SDLK_s) { gRom.save(); return true; }

    if(key >= SDLK_1 && key <= SDLK_8)
    {
        editor.level = static_cast<int>(key - SDLK_1);
        return true;
    }

    if(key == SDLK_0)
    {
        editor.mode = (editor.mode == 0) ? 1 : 0;
        return true;
    }

    if(editor.zoom == editor::ZoomMode::Screen)
    {
        if(key == SDLK_KP_8 || key == SDLK_RIGHTBRACKET)
        {
            unsigned char block = gLevelManager.blockAt(editor.level, editor.mode, currentSelectedX, currentSelectedY);
            if(block == 0xFF) { block = 0x00; }
            else if(block == gLevelManager.highestBlockID(editor.level, editor.mode)) { block = 0x00; }
            else { block++; }
            gLevelManager.setBlockAt(editor.level, editor.mode, currentSelectedX, currentSelectedY, block);
            return true;
        }

        if(key == SDLK_KP_2 || key == SDLK_LEFTBRACKET)
        {
            unsigned char block = gLevelManager.blockAt(editor.level, editor.mode, currentSelectedX, currentSelectedY);
            if(block == 0x00) { block = gLevelManager.highestBlockID(editor.level, editor.mode); }
            else { block--; }
            gLevelManager.setBlockAt(editor.level, editor.mode, currentSelectedX, currentSelectedY, block);
            return true;
        }

        if(key == SDLK_c)
        {
            gLevelManager.setBlockClipboard(
                gLevelManager.blockAt(editor.level, editor.mode, currentSelectedX, currentSelectedY)
            );
            return true;
        }

        if(key == SDLK_v)
        {
            gLevelManager.setBlockAt(editor.level, editor.mode, currentSelectedX, currentSelectedY, gLevelManager.blockClipboard());
            return true;
        }

        if(key == SDLK_KP_PLUS || key == SDLK_EQUALS)
        {
            unsigned short spx = (currentSelectedX * 4) + 1;
            unsigned short spy = (currentSelectedY * 4) + 1;
            gLevelManager.setSpawnPoint(editor.level, editor.mode, spx, spy);
            return true;
        }

        if(key == SDLK_t)
        {
            unsigned short sbx = (currentSelectedX * 4);
            unsigned short sby = (currentSelectedY * 4);
            for(int tx = 0; tx < 4; ++tx)
            {
                for(int ty = 0; ty < 4; ++ty)
                {
                    short thing = gLevelManager.thingAt(editor.level, editor.mode, sbx + tx, sby + ty);
                    if(thing >= 0) { printf("Thing found: %d\n", thing); }
                    editor.thing = static_cast<unsigned short>(thing < 0 ? 0 : thing);
                }
            }
            return true;
        }
    }

    return true;
}

int Run(int argc, char **argv)
{
    printf("blaster mapper v0.0.2\n");

    if(argc < 2)
    {
        printf("Please specify the ROM file to open.\n");
        return 1;
    }

    InitializePalette();

    if(!gRom.load(argv[1]))
    {
        printf("Failed to load ROM.\n");
        return 1;
    }

    for(int lvl = 0; lvl < 8; ++lvl)
    {
        for(int mode = 0; mode < 2; ++mode)
        {
            char option[12] = {};
            std::snprintf(option, sizeof(option), "things%d%d", lvl + 1, mode);
            if(IsCmdOptionSet(argc, argv, option)) { gRom.printThings(lvl, mode); }
        }
    }

    editor::State editor;
    ApplyCommandLineOptions(editor, argc, argv);

    if(IsCmdOptionSet(argc, argv, "print-spawn")
       || IsCmdOptionSet(argc, argv, "print-spawns")
       || IsCmdOptionSet(argc, argv, "spawnpoints"))
    {
        gLevelManager.printSpawnPoints();
    }

    if(IsCmdOptionSet(argc, argv, "save"))
    {
        gRom.save();
        return 0;
    }

    if(IsCmdOptionSet(argc, argv, "merge"))
    {
        if(argc < 5)
        {
            printf("Merge option must include a source, destination, and bank.\n");
            printf("./blastmap <sourcerom> merge <destinationrom> <bank>\n");
            return 1;
        }
        short bank = static_cast<short>(std::atoi(argv[4]));
        gRom.merge(argv[1], argv[3], bank);
        return 0;
    }

    platform::Window window;
    if(!window.initialize()) { return 1; }
    window.setupGL();

    view::EditorRenderer renderer;

    printf("Creating textures from CHR ROM\n");
    for(int el = 0; el < 8; el++)
    {
        gRom.loadUSBTextures(el, 0);
        gRom.loadUSBTextures(el, 1);
    }

    renderer.Render(editor);
    window.swapBuffers();

    bool running = true;
    SDL_Event sdlEvent;

    while(running)
    {
        while(SDL_PollEvent(&sdlEvent))
        {
            if(sdlEvent.type == SDL_QUIT)
            {
                running = false;
                break;
            }

            if(sdlEvent.type == SDL_WINDOWEVENT)
            {
                if(sdlEvent.window.event == SDL_WINDOWEVENT_SIZE_CHANGED ||
                   sdlEvent.window.event == SDL_WINDOWEVENT_RESIZED)
                {
                    window.resize(sdlEvent.window.data1, sdlEvent.window.data2);
                    glViewport(0, 0, window.width(), window.height());
                    renderer.Render(editor);
                    window.swapBuffers();
                }
            }
            else if(sdlEvent.type == SDL_KEYDOWN)
            {
                bool shouldContinue = HandleKeyPress(editor, sdlEvent.key.keysym.sym, renderer);
                renderer.Render(editor);
                window.swapBuffers();
                if(!shouldContinue)
                {
                    running = false;
                    break;
                }
            }
        }

        if(!running) { break; }
        SDL_Delay(1);
    }

    window.teardown();

    return 0;
}

} // namespace blastmap

int main(int argc, char **argv)
{
    return blastmap::Run(argc, argv);
}
