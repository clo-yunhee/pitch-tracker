#include <iostream>
#include <cstdlib>

#include "sdl2/sdl2.h"
#include "app/app.h"

int main(int argc, char *argv[])
{
    std::cout << "== Pitch tracker beta" << std::endl;

    SDL2::Context sdl(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_TIMER);

    sdl.createWindow(
            "Pitch tracker beta",
            SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
            853, 480,
            SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE,
            SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC
    );
    
    App::Context app(16'000);

    while (app.shouldContinue()) {
        SDL_Event ev;
        while (SDL_PollEvent(&ev)) {
            app.handleEvent(&ev);
        }

        sdl.renderClear();
        app.renderApp(&sdl);
        sdl.renderPresent();
    }

    return EXIT_SUCCESS;
}
