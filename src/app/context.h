#ifndef APP_CONTEXT_H
#define APP_CONTEXT_H

#include <SDL2/SDL.h>
#include <atomic>

namespace SDL2
{
    class Context;
    class Audio;
    class Font;
}

namespace Tracker
{
    class Context;
}

namespace App
{
    enum PitchLimitMode { PITCHLIMIT_MIN, PITCHLIMIT_MAX };

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
        void handleMousewheelEvent(const int y);

        std::atomic<bool> mContinue;

        SDL2::Audio *mAudio;
        Tracker::Context *mTrackerContext;

        SDL2::Font *mUiFontBig;
        SDL2::Font *mUiFontMedium;
        SDL2::Font *mUiFontSmall;
        SDL2::Font *mUiFontTiny;

        PitchLimitMode mPitchLimitMode;
        int mPitchLimit;

        int mBgLastTime;
        int mBgFadeTime;
        float mBgFrac;
        Uint8 mBgAlpha;

    };
}

#endif // APP_CONTEXT_H
