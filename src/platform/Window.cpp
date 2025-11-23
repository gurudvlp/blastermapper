#include "Window.hpp"

#include "../level/LevelInfo.hpp"
#include "../main.hpp"

#include <cstdio>

namespace blastmap {
namespace platform {

Window::Window()
    : window_(nullptr), glContext_(nullptr), width_(0), height_(0), initialized_(false)
{
}

Window::~Window()
{
    teardown();
}

bool Window::initialize()
{
    if(SDL_Init(SDL_INIT_VIDEO) != 0)
    {
        std::fprintf(stderr, "SDL_Init failed: %s\n", SDL_GetError());
        return false;
    }

    initialized_ = true;
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

    width_ = WindowWidth;
    height_ = WindowHeight;

    window_ = SDL_CreateWindow("blaster mapper",
                               SDL_WINDOWPOS_CENTERED,
                               SDL_WINDOWPOS_CENTERED,
                               width_,
                               height_,
                               SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
    if(!window_)
    {
        std::fprintf(stderr, "SDL_CreateWindow failed: %s\n", SDL_GetError());
        teardown();
        return false;
    }

    glContext_ = SDL_GL_CreateContext(window_);
    if(!glContext_)
    {
        std::fprintf(stderr, "SDL_GL_CreateContext failed: %s\n", SDL_GetError());
        teardown();
        return false;
    }

    SDL_GL_MakeCurrent(window_, glContext_);
    SDL_GL_SetSwapInterval(1);
    glEnable(GL_DEPTH_TEST);

    return true;
}

void Window::setupGL()
{
    glClearColor(1.0, 1.0, 1.0, 1.0);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-1.0, 1.0, -1.0, 1.0, 1.0, 20.0);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(0.0, 0.0, 10.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);

    glEnable(GL_TEXTURE_2D);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_BLEND);

    buildDarkenTexture();
    buildSpawnPointTexture();
    buildThingSpawnTexture();
}

void Window::teardown()
{
    if(glContext_)
    {
        SDL_GL_DeleteContext(glContext_);
        glContext_ = nullptr;
    }

    if(window_)
    {
        SDL_DestroyWindow(window_);
        window_ = nullptr;
    }

    if(initialized_)
    {
        SDL_Quit();
        initialized_ = false;
    }
}

void Window::swapBuffers() const
{
    if(window_) { SDL_GL_SwapWindow(window_); }
}

void Window::resize(int width, int height)
{
    width_ = width;
    height_ = height;
}

SDL_Window *Window::nativeWindow() const
{
    return window_;
}

int Window::width() const
{
    return width_;
}

int Window::height() const
{
    return height_;
}

void Window::buildDarkenTexture()
{
    for(int c = 0; c < (8 * 8); ++c)
    {
        darkenTex_[c * 4] = 0x00;
        darkenTex_[(c * 4) + 1] = 0x00;
        darkenTex_[(c * 4) + 2] = 0x00;
        darkenTex_[(c * 4) + 3] = 0x80;
    }

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    GLuint textureID = 0;
    glGenTextures(1, &textureID);
    level::gLevelManager.setDarkenTextureID(textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 8, 8, 0, GL_RGBA, GL_UNSIGNED_BYTE, darkenTex_);
}

void Window::buildSpawnPointTexture()
{
    for(int c = 0; c < 8; ++c)
    {
        spawnPointTex_[(c * 32) + 12] = 0x00;
        spawnPointTex_[(c * 32) + 13] = 0xC0;
        spawnPointTex_[(c * 32) + 14] = 0x00;
        spawnPointTex_[(c * 32) + 15] = 0x80;

        spawnPointTex_[(c * 32) + 16] = 0x00;
        spawnPointTex_[(c * 32) + 17] = 0xC0;
        spawnPointTex_[(c * 32) + 18] = 0x00;
        spawnPointTex_[(c * 32) + 19] = 0x80;

        spawnPointTex_[96 + (c * 4)] = 0x00;
        spawnPointTex_[96 + (c * 4) + 1] = 0xC0;
        spawnPointTex_[96 + (c * 4) + 2] = 0x00;
        spawnPointTex_[96 + (c * 4) + 3] = 0x80;

        spawnPointTex_[96 + (c * 4) + 3] = 0x80;

        spawnPointTex_[128 + (c * 4)] = 0x00;
        spawnPointTex_[128 + (c * 4) + 1] = 0xC0;
        spawnPointTex_[128 + (c * 4) + 2] = 0x00;
        spawnPointTex_[128 + (c * 4) + 3] = 0x80;
    }

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    GLuint textureID = 0;
    glGenTextures(1, &textureID);
    level::gLevelManager.setSpawnPointTextureID(textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 8, 8, 0, GL_RGBA, GL_UNSIGNED_BYTE, spawnPointTex_);
}

void Window::buildThingSpawnTexture()
{
    for(int c = 0; c < 8; ++c)
    {
        thingSpawnTex_[(c * 32) + 12] = 0xC0;
        thingSpawnTex_[(c * 32) + 13] = 0x00;
        thingSpawnTex_[(c * 32) + 14] = 0x00;
        thingSpawnTex_[(c * 32) + 15] = 0x80;

        thingSpawnTex_[(c * 32) + 16] = 0xC0;
        thingSpawnTex_[(c * 32) + 17] = 0x00;
        thingSpawnTex_[(c * 32) + 18] = 0x00;
        thingSpawnTex_[(c * 32) + 19] = 0x80;

        thingSpawnTex_[96 + (c * 4)] = 0xC0;
        thingSpawnTex_[96 + (c * 4) + 1] = 0x00;
        thingSpawnTex_[96 + (c * 4) + 2] = 0x00;
        thingSpawnTex_[96 + (c * 4) + 3] = 0x80;

        thingSpawnTex_[128 + (c * 4)] = 0xC0;
        thingSpawnTex_[128 + (c * 4) + 1] = 0x00;
        thingSpawnTex_[128 + (c * 4) + 2] = 0x00;
        thingSpawnTex_[128 + (c * 4) + 3] = 0x80;
    }

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    GLuint textureID = 0;
    glGenTextures(1, &textureID);
    level::gLevelManager.setThingSpawnTextureID(textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 8, 8, 0, GL_RGBA, GL_UNSIGNED_BYTE, thingSpawnTex_);
}

} // namespace platform
} // namespace blastmap
