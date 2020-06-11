#include <iostream>
#include <cstdlib>

#include "sdl2/sdl2.h"

int main(int argc, char *argv[])
{
    std::cout << "== Pitch tracker beta" << std::endl;

    SDL2::Context sdl(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_TIMER);

    sdl.createWindow(
            "Pitch tracker beta",
            SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
            640, 480,
            SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE,
            SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC
    );

    SDL_Event ev;
    bool quit = false;

    while (!quit) {
        while (SDL_PollEvent(&ev)) {
            if (ev.type == SDL_QUIT) {
                quit = true;
                break;
            }
        }

        sdl.renderClear();
        sdl.renderPresent();
    }

    return EXIT_SUCCESS;
}
