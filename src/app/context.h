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
        Context(SDL2::Context *sdl, int sampleRate);
        ~Context();

        void handleIdle(SDL2::Context *sdl);
        void handleEvent(SDL2::Context *sdl, const SDL_Event *ev);
        void renderApp(SDL2::Context *sdl);

        bool shouldContinue() const;
        bool isPaused() const;

    private:
        void handleKeydownEvent(const SDL_Keycode key);
        void handleKeyupEvent(const SDL_Keycode key);
        void handleMousewheelEvent(const int y);
        void handleFingermotionEvent(SDL2::Context *sdl, const float dx, const float dy);
        void handleFingerdownEvent(const Uint32 timestamp, const float x, const float y);
        void handleFingerupEvent(const Uint32 timestamp, const float x, const float y);

        void setPitchLimit(const int newPitchLimit);
        void togglePitchLimitMode();

        void togglePitchGraph();
        void renderUnscaled(SDL_Renderer *renderer, SDL_Texture *texture, SDL_Rect dst,
                            int targetWidth, int targetHeight,
                            int windowWidth, int windowHeight);

        std::atomic<bool> mContinue;
        std::atomic<bool> mPaused;

        SDL2::Audio *mAudio;
        Tracker::Context *mTrackerContext;

        int mCurrentDpi;
        int mTargetDpi;
        
        int mUiSizeBig;
        int mUiSizeMedium;
        int mUiSizeSmall;
        int mUiSizeTiny;

        SDL2::Font *mUiFontBig;
        SDL2::Font *mUiFontMedium;
        SDL2::Font *mUiFontSmall;
        SDL2::Font *mUiFontTiny;

        PitchLimitMode mPitchLimitMode;
        int mPitchLimit;

        bool mShowGraph;

        int mBgLastTime;
        int mBgFadeTime;
        float mBgFrac;
        Uint8 mBgAlpha;

        Uint32 mTouchTime;
        int mTouchAbsDx, mTouchAbsDy;
    };
}

#endif // APP_CONTEXT_H
