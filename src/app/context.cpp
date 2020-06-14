#include "app.h"
#include "../sdl2/sdl2.h"
#include "../tracker/tracker.h"
#include <iostream>

App::Context::Context(int sampleRate)
    : mContinue(true),
      mAudio(new SDL2::Audio(sampleRate)),
      mTrackerContext(new Tracker::Context(sampleRate))
{
    mAudio->addListener(mTrackerContext);
    mAudio->resume();
}

App::Context::~Context()
{
    delete mAudio;
    delete mTrackerContext;
}

void App::Context::handleEvent(const SDL_Event *ev)
{
    if (ev->type == SDL_QUIT
            || (ev->type == SDL_KEYDOWN && ev->key.keysym.sym == SDLK_ESCAPE)) {
        mContinue.store(false);
        return;
    }

    switch (ev->type) {
    case SDL_KEYDOWN:
        handleKeydownEvent(ev->key.keysym.sym);
        break;
    case SDL_KEYUP:
        handleKeyupEvent(ev->key.keysym.sym);
        break;
    default:
        break;
    }
}

void App::Context::renderApp(SDL2::Context *sdl)
{
    
}

bool App::Context::shouldContinue() const
{
    return mContinue.load();
}

void App::Context::handleKeydownEvent(const SDL_Keycode key)
{
}

void App::Context::handleKeyupEvent(const SDL_Keycode key)
{
}
