#include <array>
#include <cstdio>
#include <cstdlib>
#include <cstring>

#include <unistd.h>
#include <X11/X.h>
#include <X11/Xlib.h>
#include <GL/gl.h>
#include <GL/glx.h>
#include <GL/glu.h>

#include "main.hpp"
#include "rom/Rom.hpp"
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
namespace detail {

enum class ZoomMode : int
{
    Map = 0,
    Quadrant = 1,
    Screen = 2,
    Block = 3,
};

struct EditorState
{
    ZoomMode zoom = ZoomMode::Map;
    int level = 0;
    unsigned char mode = 0x00;
    unsigned char quadrant = 0x00;
    unsigned short x = 0;
    unsigned short y = 0;
    unsigned short xSelect = 0;
    unsigned short ySelect = 0;
    unsigned short thing = 0;
    std::array<std::array<unsigned short, 4>, 4> coords = {};

    void saveCoords()
    {
        auto &slot = coords[static_cast<int>(zoom)];
        slot[0] = x;
        slot[1] = y;
        slot[2] = xSelect;
        slot[3] = ySelect;
    }

    void restoreCoords(ZoomMode target)
    {
        auto &slot = coords[static_cast<int>(target)];
        x = slot[0];
        y = slot[1];
        xSelect = slot[2];
        ySelect = slot[3];
    }

    void zoomIn()
    {
        if(zoom == ZoomMode::Block) { return; }
        saveCoords();
        zoom = static_cast<ZoomMode>(static_cast<int>(zoom) + 1);

        if(zoom == ZoomMode::Quadrant)
        {
            x = xSelect;
            y = ySelect;
        }
        else if(zoom == ZoomMode::Screen)
        {
            x = (x == 1) ? (4 + xSelect) : xSelect;
            y = (y == 1) ? (4 + ySelect) : ySelect;
        }

        xSelect = 0;
        ySelect = 0;
    }

    void zoomOut()
    {
        if(zoom == ZoomMode::Map) { return; }
        zoom = static_cast<ZoomMode>(static_cast<int>(zoom) - 1);
        restoreCoords(zoom);
    }

    int selectionMax() const
    {
        switch(zoom)
        {
            case ZoomMode::Map:
            case ZoomMode::Block:
                return 1;
            case ZoomMode::Quadrant:
            case ZoomMode::Screen:
                return 3;
        }
        return 1;
    }

    void moveLeft()
    {
        if(xSelect == 0) { xSelect = selectionMax(); }
        else { --xSelect; }
    }

    void moveRight()
    {
        if(xSelect == selectionMax()) { xSelect = 0; }
        else { ++xSelect; }
    }

    void moveUp()
    {
        if(ySelect == 0) { ySelect = selectionMax(); }
        else { --ySelect; }
    }

    void moveDown()
    {
        if(ySelect == selectionMax()) { ySelect = 0; }
        else { ++ySelect; }
    }
};

} // namespace detail

namespace platform {

Display *dpy = nullptr;
Window root;
GLint att[] = { GLX_RGBA, GLX_DEPTH_SIZE, 24, GLX_DOUBLEBUFFER, None };
XVisualInfo *vi = nullptr;
Colormap cmap;
XSetWindowAttributes swa;
Window win;
GLXContext glc;
XWindowAttributes gwa;
XEvent xev;
GLubyte darkenTex[8 * 8 * 4];
GLubyte spawnPointTex[8 * 8 * 4];
GLubyte thingSpawnTex[8 * 8 * 4];

namespace {

void BuildDarkenTexture()
{
    for(int c = 0; c < (8 * 8); ++c)
    {
        darkenTex[c * 4] = 0x00;
        darkenTex[(c * 4) + 1] = 0x00;
        darkenTex[(c * 4) + 2] = 0x00;
        darkenTex[(c * 4) + 3] = 0x80;
    }

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glGenTextures(1, &darkenTextureID);
    glBindTexture(GL_TEXTURE_2D, darkenTextureID);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 8, 8, 0, GL_RGBA, GL_UNSIGNED_BYTE, darkenTex);
}

void BuildSpawnPointTexture()
{
    for(int c = 0; c < 8; ++c)
    {
        spawnPointTex[(c * 32) + 12] = 0x00;
        spawnPointTex[(c * 32) + 13] = 0xC0;
        spawnPointTex[(c * 32) + 14] = 0x00;
        spawnPointTex[(c * 32) + 15] = 0x80;

        spawnPointTex[(c * 32) + 16] = 0x00;
        spawnPointTex[(c * 32) + 17] = 0xC0;
        spawnPointTex[(c * 32) + 18] = 0x00;
        spawnPointTex[(c * 32) + 19] = 0x80;

        spawnPointTex[96 + (c * 4)] = 0x00;
        spawnPointTex[96 + (c * 4) + 1] = 0xC0;
        spawnPointTex[96 + (c * 4) + 2] = 0x00;
        spawnPointTex[96 + (c * 4) + 3] = 0x80;

        spawnPointTex[128 + (c * 4)] = 0x00;
        spawnPointTex[128 + (c * 4) + 1] = 0xC0;
        spawnPointTex[128 + (c * 4) + 2] = 0x00;
        spawnPointTex[128 + (c * 4) + 3] = 0x80;
    }

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glGenTextures(1, &spawnPointTextureID);
    glBindTexture(GL_TEXTURE_2D, spawnPointTextureID);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 8, 8, 0, GL_RGBA, GL_UNSIGNED_BYTE, spawnPointTex);
}

void BuildThingSpawnTexture()
{
    for(int c = 0; c < 8; ++c)
    {
        thingSpawnTex[(c * 32) + 12] = 0xC0;
        thingSpawnTex[(c * 32) + 13] = 0x00;
        thingSpawnTex[(c * 32) + 14] = 0x00;
        thingSpawnTex[(c * 32) + 15] = 0x80;

        thingSpawnTex[(c * 32) + 16] = 0xC0;
        thingSpawnTex[(c * 32) + 17] = 0x00;
        thingSpawnTex[(c * 32) + 18] = 0x00;
        thingSpawnTex[(c * 32) + 19] = 0x80;

        thingSpawnTex[96 + (c * 4)] = 0xC0;
        thingSpawnTex[96 + (c * 4) + 1] = 0x00;
        thingSpawnTex[96 + (c * 4) + 2] = 0x00;
        thingSpawnTex[96 + (c * 4) + 3] = 0x80;

        thingSpawnTex[128 + (c * 4)] = 0xC0;
        thingSpawnTex[128 + (c * 4) + 1] = 0x00;
        thingSpawnTex[128 + (c * 4) + 2] = 0x00;
        thingSpawnTex[128 + (c * 4) + 3] = 0x80;
    }

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glGenTextures(1, &thingSpawnTextureID);
    glBindTexture(GL_TEXTURE_2D, thingSpawnTextureID);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 8, 8, 0, GL_RGBA, GL_UNSIGNED_BYTE, thingSpawnTex);
}

} // namespace

void SetupXWindows()
{
    dpy = XOpenDisplay(NULL);
    if(!dpy) { fprintf(stderr, "cannot connect to X server\n"); std::exit(1); }

    root = DefaultRootWindow(dpy);
    vi = glXChooseVisual(dpy, 0, att);
    if(!vi) { fprintf(stderr, "no appropriate visual found\n"); std::exit(1); }

    cmap = XCreateColormap(dpy, root, vi->visual, AllocNone);
    swa.colormap = cmap;
    swa.event_mask = ExposureMask | KeyPressMask;

    win = XCreateWindow(dpy, root, 0, 0, WindowWidth, WindowHeight, 0,
                        vi->depth, InputOutput, vi->visual,
                        CWColormap | CWEventMask, &swa);
    XMapWindow(dpy, win);
    XStoreName(dpy, win, "blaster mapper");

    glc = glXCreateContext(dpy, vi, NULL, GL_TRUE);
    glXMakeCurrent(dpy, win, glc);
    glEnable(GL_DEPTH_TEST);
}

void SetupGL()
{
    glClearColor(1.0, 1.0, 1.0, 1.0);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-1., 1., -1., 1., 1., 20.);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(0., 0., 10., 0., 0., 0., 0., 1., 0.);

    glEnable(GL_TEXTURE_2D);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_BLEND);
    BuildDarkenTexture();
    BuildSpawnPointTexture();
    BuildThingSpawnTexture();
}

} // namespace platform

namespace {
view::MapRenderer g_mapRenderer;
view::QuadrantRenderer g_quadrantRenderer;
view::ScreenRenderer g_screenRenderer;
view::BlockRenderer g_blockRenderer;
} // namespace

void RenderEditor(const detail::EditorState &state)
{
    switch(state.zoom)
    {
        case detail::ZoomMode::Map:
            g_mapRenderer.SetLevelMode(state.level, state.mode);
            g_mapRenderer.SetSelection(state.xSelect, state.ySelect);
            g_mapRenderer.Render(0x01);
            break;
        case detail::ZoomMode::Quadrant:
            g_quadrantRenderer.SetLevelMode(state.level, state.mode);
            g_quadrantRenderer.SetQuadrant(state.quadrant);
            g_quadrantRenderer.SetSelection(state.xSelect, state.ySelect);
            g_quadrantRenderer.Render(0x01);
            break;
        case detail::ZoomMode::Screen:
            g_screenRenderer.SetLevelMode(state.level, state.mode);
            g_screenRenderer.SetScreen((state.y * 8) + state.x);
            g_screenRenderer.SetSelection(state.xSelect, state.ySelect);
            g_screenRenderer.Render(0x01);
            break;
        case detail::ZoomMode::Block:
            g_blockRenderer.SetLevelMode(state.level, state.mode);
            g_blockRenderer.SetBlock((state.y * 32) + state.x);
            g_blockRenderer.Render();
            break;
    }
}

void GetCmdEditorCoords(int argc, char ** argv, detail::EditorState &state)
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

void ApplyCommandLineOptions(detail::EditorState &state, int argc, char **argv)
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

    if(IsCmdOptionSet(argc, argv, "zoommap")) { state.zoom = detail::ZoomMode::Map; }
    if(IsCmdOptionSet(argc, argv, "zoomquadrant")) { state.zoom = detail::ZoomMode::Quadrant; }
    if(IsCmdOptionSet(argc, argv, "zoomscreen")) { state.zoom = detail::ZoomMode::Screen; }
    if(IsCmdOptionSet(argc, argv, "zoomblock")) { state.zoom = detail::ZoomMode::Block; }

    GetCmdEditorCoords(argc, argv, state);

    if(IsCmdOptionSet(argc, argv, "levelpointers")) { SAVEROM_SHOW_LEVEL_POINTERS = true; }
    else { SAVEROM_SHOW_LEVEL_POINTERS = false; }
}

void HandleKeyPress(detail::EditorState &editor, XKeyEvent *xke)
{
    const auto code = xke->keycode;
    auto &screenRenderer = g_screenRenderer;
    const unsigned char currentSelectedX = screenRenderer.SelectedBlockX();
    const unsigned char currentSelectedY = screenRenderer.SelectedBlockY();

    if(code == 9)
    {
        if(editor.zoom == detail::ZoomMode::Map)
        {
            glXMakeCurrent(platform::dpy, None, NULL);
            glXDestroyContext(platform::dpy, platform::glc);
            XDestroyWindow(platform::dpy, platform::win);
            XCloseDisplay(platform::dpy);
            std::exit(0);
        }
        editor.zoomOut();
        return;
    }

    if(code == 111) { editor.moveUp(); return; }
    if(code == 116) { editor.moveDown(); return; }
    if(code == 113) { editor.moveLeft(); return; }
    if(code == 114) { editor.moveRight(); return; }

    if(code == 36) { editor.zoomIn(); return; }
    if(code == 39) { rom::SaveRom(); return; }

    if(code >= 10 && code <= 17)
    {
        editor.level = code - 10;
        return;
    }

    if(code == 19)
    {
        editor.mode = (editor.mode == 0) ? 1 : 0;
        return;
    }

    if(editor.zoom == detail::ZoomMode::Screen)
    {
        if(code == 80 || code == 35)
        {
            unsigned char block = BlockAt(editor.level, editor.mode, currentSelectedX, currentSelectedY);
            if(block == 0xFF) { block = 0x00; }
            else if(block == GetHighestBlockID(editor.level, editor.mode)) { block = 0x00; }
            else { block++; }
            SetBlockAt(editor.level, editor.mode, currentSelectedX, currentSelectedY, block);
            return;
        }

        if(code == 88 || code == 34)
        {
            unsigned char block = BlockAt(editor.level, editor.mode, currentSelectedX, currentSelectedY);
            if(block == 0x00) { block = GetHighestBlockID(editor.level, editor.mode); }
            else { block--; }
            SetBlockAt(editor.level, editor.mode, currentSelectedX, currentSelectedY, block);
            return;
        }

        if(code == 54)
        {
            BlockClipboard = BlockAt(editor.level, editor.mode, currentSelectedX, currentSelectedY);
            return;
        }

        if(code == 55)
        {
            SetBlockAt(editor.level, editor.mode, currentSelectedX, currentSelectedY, BlockClipboard);
            return;
        }

        if(code == 86 || code == 21)
        {
            unsigned short spx = (currentSelectedX * 4) + 1;
            unsigned short spy = (currentSelectedY * 4) + 1;
            SetSpawnPoint(editor.level, editor.mode, spx, spy);
            return;
        }

        if(code == 28)
        {
            unsigned short sbx = (currentSelectedX * 4);
            unsigned short sby = (currentSelectedY * 4);
            for(int tx = 0; tx < 4; ++tx)
            {
                for(int ty = 0; ty < 4; ++ty)
                {
                    short thing = GetThingAt(editor.level, editor.mode, sbx + tx, sby + ty);
                    if(thing >= 0) { printf("Thing found: %d\n", thing); }
                    editor.thing = static_cast<unsigned short>(thing < 0 ? 0 : thing);
                }
            }
            return;
        }
    }
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

    if(!rom::LoadRom(argv[1]))
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
            if(IsCmdOptionSet(argc, argv, option)) { rom::PrintThings(lvl, mode); }
        }
    }

    for(int el = 0; el < 8; ++el)
    {
        rom::LoadUSBTextures(el, 0);
        rom::LoadUSBTextures(el, 1);
    }

    detail::EditorState editor;
    ApplyCommandLineOptions(editor, argc, argv);

    if(IsCmdOptionSet(argc, argv, "print-spawn")
       || IsCmdOptionSet(argc, argv, "print-spawns")
       || IsCmdOptionSet(argc, argv, "spawnpoints"))
    {
        PrintSpawnPoints();
    }

    if(IsCmdOptionSet(argc, argv, "save"))
    {
        rom::SaveRom();
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
        rom::Merge(argv[1], argv[3], bank);
        return 0;
    }

    platform::SetupXWindows();
    platform::SetupGL();

    printf("Creating textures from CHR ROM\n");
    int el;
	for(el = 0; el < 8; el++)
	{
		rom::LoadUSBTextures(el, 0);
		rom::LoadUSBTextures(el, 1);
	}

    while(true)
    {
        while(XPending(platform::dpy))
        {
            XNextEvent(platform::dpy, &platform::xev);

            if(platform::xev.type == Expose)
            {
                printf("Expose event\n");
                XGetWindowAttributes(platform::dpy, platform::win, &platform::gwa);
                glViewport(0, 0, platform::gwa.width, platform::gwa.height);
                
                RenderEditor(editor);
                glXSwapBuffers(platform::dpy, platform::win);
            }
            else if(platform::xev.type == KeyPress)
            {
                XKeyEvent *xke = reinterpret_cast<XKeyEvent *>(&platform::xev);
                printf("Keycode %d\n", xke->keycode);
                HandleKeyPress(editor, xke);
                
                RenderEditor(editor);
                glXSwapBuffers(platform::dpy, platform::win);
            }
        }
        sleep(0);
    }

    return 0;
}

} // namespace blastmap

int main(int argc, char **argv)
{
    return blastmap::Run(argc, argv);
}
