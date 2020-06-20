#include "font.h"
#include <iostream>

int SDL2::Font::sInstanceCount = 0;
std::mutex SDL2::Font::sInstanceLock;

SDL2::Font::Font(const char *name, int size)
{
    std::lock_guard<std::mutex> lock(sInstanceLock);
    if (sInstanceCount == 0) {
        if (TTF_Init() == -1) {
            std::cerr << "TTF_Init() error: " << TTF_GetError() << std::endl;
            exit(EXIT_FAILURE);
        }
    }
    mFont = TTF_OpenFont(name, size);
    if (mFont == nullptr) {
        std::cerr << "TTF_OpenFont() error: " << TTF_GetError() << std::endl;
        exit(EXIT_FAILURE);
    }
    sInstanceCount++;
}

SDL2::Font::~Font()
{
    std::lock_guard<std::mutex> lock(sInstanceLock);
    TTF_CloseFont(mFont);
    sInstanceCount--;
    if (sInstanceCount == 0) {
        TTF_Quit();
    }
}

SDL2::TexturePtr SDL2::Font::renderText(SDL_Renderer *renderer, const char *text, const SDL_Color color)
{
    SDL_Surface *surface = TTF_RenderText_Blended(mFont, text, color);
    if (surface == nullptr) {
        std::cerr << "TTF_RenderText_Blended() error: " << TTF_GetError() << std::endl;
        exit(EXIT_FAILURE);  
    }
    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);
    return SDL2::TexturePtr(texture);
}

void SDL2::Font::querySize(const char *text, int *w, int *h)
{
    if (TTF_SizeText(mFont, text, w, h) == -1) {
        std::cerr << "TTF_SizeText() error: " << TTF_GetError() << std::endl;
        exit(EXIT_FAILURE);
    }
}

int SDL2::Font::ascent()
{
    return TTF_FontAscent(mFont);
}

int SDL2::Font::descent()
{
    return TTF_FontDescent(mFont);
}
