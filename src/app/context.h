#ifndef APP_CONTEXT_H
#define APP_CONTEXT_H

#include <SDL2/SDL.h>
#include <atomic>

namespace SDL2
{
    class Context;
    class Audio;
}

namespace Tracker
{
    class Context;
}

namespace App
{
    class Context
    {
    public:
        Context(int sampleRate);
        ~Context();

        void handleEvent(const SDL_Event *ev);
        void renderApp(SDL2::Context *sdl);

        bool shouldContinue() const;

    private:
        void handleKeydownEvent(const SDL_Keycode key);
        void handleKeyupEvent(const SDL_Keycode key);

        std::atomic<bool> mContinue;

        SDL2::Audio *mAudio;
        Tracker::Context *mTrackerContext;
    };
}

#endif // APP_CONTEXT_H
