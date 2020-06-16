#ifndef SDL2_SDL2_H
#define SDL2_SDL2_H

#include <SDL2/SDL.h>
#include <memory>

namespace SDL2
{
    struct TextureDeletor
    {
        void operator()(SDL_Texture *tex) {
            SDL_DestroyTexture(tex);
        }
    };

    using TexturePtr = std::unique_ptr<SDL_Texture, SDL2::TextureDeletor>;
}

#include "context.h"
#include "audio.h"
#include "font.h"

#endif // SDL2_SDL2_H
