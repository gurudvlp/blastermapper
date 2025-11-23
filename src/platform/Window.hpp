#ifndef BLASTMAP_PLATFORM_WINDOW_HPP
#define BLASTMAP_PLATFORM_WINDOW_HPP

#include <SDL2/SDL.h>
#include <GL/gl.h>
#include <GL/glu.h>

namespace blastmap {
namespace platform {

class Window
{
public:
    Window();
    ~Window();

    Window(const Window &) = delete;
    Window &operator=(const Window &) = delete;
    Window(Window &&) = delete;
    Window &operator=(Window &&) = delete;

    bool initialize();
    void setupGL();
    void teardown();
    void swapBuffers() const;
    void resize(int width, int height);

    SDL_Window *nativeWindow() const;
    int width() const;
    int height() const;

private:
    void buildDarkenTexture();
    void buildSpawnPointTexture();
    void buildThingSpawnTexture();

    SDL_Window *window_;
    SDL_GLContext glContext_;
    int width_;
    int height_;
    bool initialized_;
    GLubyte darkenTex_[8 * 8 * 4];
    GLubyte spawnPointTex_[8 * 8 * 4];
    GLubyte thingSpawnTex_[8 * 8 * 4];
};

} // namespace platform
} // namespace blastmap

#endif // BLASTMAP_PLATFORM_WINDOW_HPP
