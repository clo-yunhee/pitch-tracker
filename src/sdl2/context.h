#ifndef SDL2_CONTEXT_H
#define SDL2_CONTEXT_H

#include <SDL2/SDL.h>

namespace SDL2
{

    class Context
    {
    public:
        Context(Uint32 initFlags);
        ~Context();

        void createWindow(const char *title, int x, int y, int w, int h, Uint32 windowFlags, Uint32 rendererFlags);
        void destroyWindow();
       
        void renderClear();
        void renderPresent();

    private:
        SDL_Window *mWindow;
        SDL_Renderer *mRenderer;

    };

}

#endif // SDL2_CONTEXT_H
