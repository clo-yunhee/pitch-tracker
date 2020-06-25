#ifndef SDL2_CONTEXT_H
#define SDL2_CONTEXT_H

#include <SDL2/SDL.h>
#include "../app/app.h"

namespace SDL2
{
    class Context
    {
    public:
        Context(Uint32 initFlags);
        ~Context();

        void createWindow(const char *title, int x, int y, int w, int h, Uint32 windowFlags, Uint32 rendererFlags);
        void destroyWindow();

        void openHaptic();
        void closeHaptic();
        void rumblePlay(float strength, Uint32 length);
        void rumbleStop();
       
        void renderClear();
        void renderPresent();

        float windowDisplayDpi();
        int dipSizeInPixels(int dip);
        void windowSize(int *w, int *h);

    private:
        inline SDL_Window *window() { return mWindow; }
        inline SDL_Renderer *renderer() { return mRenderer; }

        SDL_Window *mWindow;
        SDL_Renderer *mRenderer;

        SDL_Haptic *mHaptic;
        
        friend void App::Context::renderApp(SDL2::Context *sdl);
    };
}

#endif // SDL2_CONTEXT_H
