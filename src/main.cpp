#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <memory>

#include <SDL2/SDL.h>
#include <GL/gl.h>
#include <GL/glu.h>

#include "main.hpp"
#include "Engine.hpp"
#include "input/InputService.hpp"
#include "editor/EditorState.hpp"
#include "rom/Rom.hpp"
#include "view/EditorRenderer.hpp"
#include "view/Palette.hpp"
#include "window/WindowService.hpp"
#include "event/SDLEventService.hpp"
#include "level/LevelInfo.hpp"
#include "ui/UIService.hpp"

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

    if(IsCmdOptionSet(argc, argv, "levelpointers")) { SAVEROM_SHOW_LEVEL_POINTERS = true; }
    else { SAVEROM_SHOW_LEVEL_POINTERS = false; }
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

    view::EditorRenderer renderer;
    Engine engine(renderer, editor);
    auto windowService = std::make_unique<window::WindowService>(engine.eventManager());
    if(!windowService->initialize()) { return 1; }
    engine.registerService(std::move(windowService));

    printf("Creating textures from CHR ROM\n");
    for(int el = 0; el < 8; el++)
    {
        gRom.loadUSBTextures(el, 0);
        gRom.loadUSBTextures(el, 1);
    }
    auto sdlEventService =
        std::make_unique<event::SDLEventService>(engine.eventManager());
    engine.registerService(std::move(sdlEventService));
    auto uiService =
        std::make_unique<ui::UIService>(engine.eventManager(), editor, level::gLevelManager);
    ui::UIService *uiServicePtr = uiService.get();
    engine.registerService(std::move(uiService));
    auto inputService =
        std::make_unique<input::InputService>(editor, gRom, engine.eventManager());
    inputService->setUIService(uiServicePtr);
    engine.registerService(std::move(inputService));
    engine.run();

    return 0;
}

} // namespace blastmap

int main(int argc, char **argv)
{
    return blastmap::Run(argc, argv);
}
