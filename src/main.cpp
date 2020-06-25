#include <iostream>
#include <cstdlib>

#include "sdl2/sdl2.h"
#include "app/app.h"

int main(int argc, char *argv[])
{
    std::cout << "== Pitch tracker beta" << std::endl;

    SDL2::Context sdl(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_TIMER | SDL_INIT_HAPTIC);

    sdl.createWindow(
            "Pitch tracker beta",
            SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
            853, 480,
            SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE,
            SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC
    );

    sdl.openHaptic();

    App::Context app(&sdl, 16'000);

    while (app.shouldContinue()) { 
        SDL_Event ev;
        while (SDL_PollEvent(&ev)) {
            app.handleEvent(&sdl, &ev);
        }

        if (!app.isPaused()) {
            sdl.renderClear();
            app.renderApp(&sdl);
            sdl.renderPresent();
        }
    }

    return EXIT_SUCCESS;
}
