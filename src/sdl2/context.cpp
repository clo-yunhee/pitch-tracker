#include "context.h"
#include <iostream>

SDL2::Context::Context(Uint32 initFlags)
    : mWindow(nullptr), mRenderer(nullptr), mHaptic(nullptr)
{
    if (SDL_Init(initFlags) != 0) {
        std::cerr << "SDL_Init() error: " << SDL_GetError() << std::endl;
        exit(EXIT_FAILURE);
    }

}

SDL2::Context::~Context()
{
    closeHaptic();
    destroyWindow(); 
    SDL_Quit();
}

void SDL2::Context::createWindow(const char *title, int x, int y, int w, int h, Uint32 windowFlags, Uint32 rendererFlags)
{
    if (mWindow != nullptr || mRenderer != nullptr) {
        std::cerr << "Window or renderer already created" << std::endl;
        exit(EXIT_FAILURE);
    }

#ifdef __ANDROID__
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
#endif

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

void SDL2::Context::openHaptic()
{
    if (SDL_NumHaptics() >= 1) {
        mHaptic = SDL_HapticOpen(0);
        if (!(SDL_HapticRumbleSupported(mHaptic) && SDL_HapticRumbleInit(mHaptic) == 0)) {
            SDL_HapticClose(mHaptic);
            mHaptic = nullptr;
            return;
        }
    }
}

void SDL2::Context::closeHaptic()
{
    if (mHaptic != nullptr) {
        SDL_HapticRumbleStop(mHaptic);
        SDL_HapticClose(mHaptic);
    }
}

void SDL2::Context::rumblePlay(float strength, Uint32 length)
{
    if (mHaptic != nullptr) {
        SDL_HapticRumblePlay(mHaptic, strength, length);
    }
}

void SDL2::Context::rumbleStop()
{
    if (mHaptic != nullptr) {
        SDL_HapticRumbleStop(mHaptic);
    }
}

void SDL2::Context::renderClear()
{
    if (mRenderer != nullptr) {
        SDL_RenderClear(mRenderer);
        
        SDL_SetRenderDrawColor(mRenderer, 0, 0, 0, 255);

        SDL_Rect dst;
        dst.x = dst.y = 0;
        windowSize(&dst.w, &dst.h);
        SDL_RenderFillRect(mRenderer, &dst);
    }
}

void SDL2::Context::renderPresent()
{
    if (mRenderer != nullptr) {
        SDL_RenderPresent(mRenderer);
    }
}

float SDL2::Context::windowDisplayDpi()
{
    int index = SDL_GetWindowDisplayIndex(mWindow);
    if (index < 0) {
        std::cerr << "SDL_GetWindowDisplayIndex() error: " << SDL_GetError() << std::endl;
        exit(EXIT_FAILURE);
    }
    
    float dpi;
    if (SDL_GetDisplayDPI(index, nullptr, nullptr, &dpi) != 0) {
        std::cerr << "SDL_GetDisplayDPI() error: " << SDL_GetError() << std::endl;
        exit(EXIT_FAILURE);
    }

    return dpi;
}

int SDL2::Context::dipSizeInPixels(int dip)
{
    return (int) (dip * windowDisplayDpi() / 140.0F);
    
}

void SDL2::Context::windowSize(int *w, int *h)
{
    SDL_GetWindowSize(mWindow, w, h);
}
