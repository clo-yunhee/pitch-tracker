#ifndef SDL2_FONT_H
#define SDL2_FONT_H

#include "sdl2.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <memory>
#include <mutex>

namespace SDL2
{
    class Font
    {
    public:
        Font(const char *name, int size);
        ~Font();

        SDL2::TexturePtr renderText(SDL_Renderer *renderer, const char *text, const SDL_Color color);
    
        void querySize(const char *text, int *w, int *h);

        int ascent();
        int descent();

    private:
        TTF_Font *mFont;

        static int sInstanceCount;
        static std::mutex sInstanceLock;
    };
}

#endif // SDL2_FONT_H
