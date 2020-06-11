#include "context.h"
#include <iostream>

SDL2::Context::Context(Uint32 initFlags)
    : mWindow(nullptr), mRenderer(nullptr)
{
    if (SDL_Init(initFlags) != 0) {
        std::cerr << "SDL_Init() error: " << SDL_GetError() << std::endl;
        exit(EXIT_FAILURE);
    }
}

SDL2::Context::~Context()
{
    destroyWindow(); 
    SDL_Quit();
}

void SDL2::Context::createWindow(const char *title, int x, int y, int w, int h, Uint32 windowFlags, Uint32 rendererFlags)
{
    if (mWindow != nullptr || mRenderer != nullptr) {
        std::cerr << "Window or renderer already created" << std::endl;
        exit(EXIT_FAILURE);
    }

    mWindow = SDL_CreateWindow(title, x, y, w, h, windowFlags);
    if (mWindow == nullptr) {
        std::cerr << "SDL_CreateWindow() error: " << SDL_GetError() << std::endl;
        exit(EXIT_FAILURE);
    }

    mRenderer = SDL_CreateRenderer(mWindow, -1, rendererFlags);
    if (mRenderer == nullptr) {
        std::cerr << "SDL_CreateRenderer() error: " << SDL_GetError() << std::endl;
        exit(EXIT_FAILURE);
    }
}

void SDL2::Context::destroyWindow()
{
    if (mRenderer != nullptr) {
        SDL_DestroyRenderer(mRenderer);
        mRenderer = nullptr;
    }

    if (mWindow != nullptr) {
        SDL_DestroyWindow(mWindow);
        mWindow = nullptr;
    }
}

void SDL2::Context::renderClear()
{
    if (mRenderer != nullptr) {
        SDL_RenderClear(mRenderer);
    }
}

void SDL2::Context::renderPresent()
{
    if (mRenderer != nullptr) {
        SDL_RenderPresent(mRenderer);
    }
}
