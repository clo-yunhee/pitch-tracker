#include "app.h"
#include "../sdl2/sdl2.h"
#include "../tracker/tracker.h"
#include <iostream>

App::Context::Context(int sampleRate)
    : mContinue(true),
      mAudio(new SDL2::Audio(sampleRate)),
      mTrackerContext(new Tracker::Context(sampleRate)),
      mUiFontBig(new SDL2::Font("Montserrat.ttf", 32)),
      mUiFontMedium(new SDL2::Font("Montserrat.ttf", 24)),
      mUiFontSmall(new SDL2::Font("Montserrat.ttf", 18)),    
      mUiFontTiny(new SDL2::Font("Montserrat.ttf", 14)),    
      mPitchLimit(170),
      mPitchLimitMode(PITCHLIMIT_MIN),
      mBgFadeTime(100),
      mBgAlpha(0),
      mBgLastTime(SDL_GetTicks())
{
    mAudio->addListener(mTrackerContext);
    mAudio->resume();
}

App::Context::~Context()
{
    delete mUiFontBig;
    delete mUiFontMedium;
    delete mUiFontSmall;
    delete mUiFontTiny;
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
    case SDL_MOUSEWHEEL:
        handleMousewheelEvent((ev->wheel.direction == SDL_MOUSEWHEEL_NORMAL) ? ev->wheel.y : -ev->wheel.y);
        break;
    default:
        break;
    }
}

void App::Context::renderApp(SDL2::Context *sdl)
{
    int targetWidth, targetHeight;
    SDL_GetWindowSize(sdl->window(), &targetWidth, &targetHeight);

    constexpr int maxLen = 64;
    char string[maxLen];
    SDL2::TexturePtr stringTex;
    int w, h;
    SDL_Rect dst;
    SDL_Color color;

    float pitch = mTrackerContext->pitch();

    int bgSgn;
    if  (pitch != 0 &&
            ((mPitchLimitMode == PITCHLIMIT_MIN && pitch <= mPitchLimit)
                || (mPitchLimitMode == PITCHLIMIT_MAX && pitch >= mPitchLimit))) {
        bgSgn = 1;
    }
    else {
        bgSgn = -1;
    }

    int now = SDL_GetTicks();
    if (now - mBgLastTime > 0) {
        mBgFrac += (float) (bgSgn * (now - mBgLastTime)) / (float) mBgFadeTime;
        mBgFrac = std::clamp(mBgFrac, 0.0F, 1.0F);
        mBgAlpha = (Uint8) round(255 * mBgFrac * mBgFrac * (3.0F - 2.0F * mBgFrac));
        mBgLastTime = now;
    }

    // Background.
    color = {40, 42, 54, 255};
    dst = {0, 0, targetWidth, targetHeight};
    SDL_SetRenderDrawColor(sdl->renderer(), color.r, color.g, color.b, color.a);
    SDL_RenderFillRect(sdl->renderer(), &dst);

    // Pitch floor color overlay.
    if (mBgAlpha >= 48) { 
        color = {255, 85, 85, Uint8(mBgAlpha / 48)};
        dst = {0, 0, targetWidth, targetHeight};
        SDL_SetRenderDrawBlendMode(sdl->renderer(), SDL_BLENDMODE_ADD);
        SDL_SetRenderDrawColor(sdl->renderer(), color.r, color.g, color.b, color.a);
        SDL_RenderFillRect(sdl->renderer(), &dst);
    }

    // Help text.
    SDL_Color helpTextLeftColor = {149, 149, 145, 255};
    SDL_Color helpTextRightColor = {255, 121, 198, 255};
    std::vector<const char *> helpText{
        "Floor/ceiling: ",
            "M / One-finger tap",
        "Limit down: ",
            "Up / Scroll up",
        "Limit up: ",
            "Down / scroll down",
        "Tracer on/off: ",
            "T / Two-finger tap",
    };
    int helpTextCount = helpText.size();
    std::vector<SDL_Rect> helpTextRect(helpTextCount);

    int helpTextMaxrw = 0;
    int helpTextMaxh = 0;

    for (int i = 0; i < helpTextCount / 2; ++i) {
        SDL_Rect &rectLeft = helpTextRect.at(i * 2);
        SDL_Rect &rectRight = helpTextRect.at(i * 2 + 1);

        mUiFontSmall->querySize(helpText[i * 2], &rectLeft.w, &rectLeft.h);
        mUiFontTiny->querySize(helpText[i * 2 + 1], &rectRight.w, &rectRight.h);

        if (rectRight.w > helpTextMaxrw) {
            helpTextMaxrw = rectRight.w;
        }
    }

    int helpTextY = 10;
    for (int i = 0; i < helpTextCount / 2; ++i) {
        SDL_Rect &rectLeft = helpTextRect.at(i * 2);
        SDL_Rect &rectRight = helpTextRect.at(i * 2 + 1);

        rectRight.x = (targetWidth - 1) - 15 - helpTextMaxrw;
        rectLeft.x = rectRight.x - rectLeft.w;

        rectLeft.y = helpTextY;
        rectRight.y = helpTextY - mUiFontSmall->descent();
    
        stringTex = mUiFontSmall->renderText(sdl->renderer(), helpText[i * 2], helpTextLeftColor);
        SDL_RenderCopy(sdl->renderer(), stringTex.get(), nullptr, &rectLeft);

        stringTex = mUiFontTiny->renderText(sdl->renderer(), helpText[i * 2 + 1], helpTextRightColor);
        SDL_RenderCopy(sdl->renderer(), stringTex.get(), nullptr, &rectRight);

        helpTextY += mUiFontSmall->lineSkip(); 
    }
    
    // Current pitch estimate.
    if (pitch != 0) {
        snprintf(string, maxLen, "%d", (int) mTrackerContext->pitch());
        color = {248, 248, 242, 255};
    }
    else {
        strncpy(string, "---", maxLen);
        color = {98, 114, 164, 255};
    }

    mUiFontBig->querySize(string, &dst.w, &dst.h);
    mUiFontMedium->querySize(" Hz", &w, &h);
    stringTex = mUiFontBig->renderText(sdl->renderer(), string, color);
    dst.x = targetWidth / 2 - (dst.w + w) / 2;
    dst.y = targetHeight / 2 - dst.h / 2;
    SDL_RenderCopy(sdl->renderer(), stringTex.get(), nullptr, &dst);
  
    color = {149, 149, 145, 255};
    stringTex = mUiFontMedium->renderText(sdl->renderer(), " Hz", color);
    dst = {dst.x + dst.w, targetHeight / 2 - h / 2, w, h};
    SDL_RenderCopy(sdl->renderer(), stringTex.get(), nullptr, &dst);

    // Pitch floor alert text.
    if (mBgAlpha >= 12) {
        if (mPitchLimitMode == PITCHLIMIT_MIN) {
            strncpy(string, "Too low!", maxLen);
        }
        else {
            strncpy(string, "Too high!", maxLen);
        }
        mUiFontSmall->querySize(string, &dst.w, &dst.h);
        color = {255, 85, 85, mBgAlpha};
        stringTex = mUiFontSmall->renderText(sdl->renderer(), string, color);
        dst.x = targetWidth / 2 - dst.w / 2;
        dst.y += dst.h + 30;
        SDL_RenderCopy(sdl->renderer(), stringTex.get(), nullptr, &dst);
    }

    // Pitch floor setting text.
    if (mPitchLimitMode == PITCHLIMIT_MIN) {
        strncpy(string, "Pitch floor: ", maxLen);
    }
    else {
        strncpy(string, "Pitch ceiling: ", maxLen);
    }

    mUiFontSmall->querySize(string, &dst.w, &dst.h);
    color = {149, 149, 145, 255};
    stringTex = mUiFontSmall->renderText(sdl->renderer(), string, color);
    dst.x = 15;
    dst.y = (targetHeight - 1) - 10 - dst.h;
    SDL_RenderCopy(sdl->renderer(), stringTex.get(), nullptr, &dst);

    snprintf(string, maxLen, "%d", mPitchLimit);
    mUiFontSmall->querySize(string, &w, &h);
    color = {248, 248, 242, 255};
    if (mBgAlpha > 8) {
        color = {255, 85, 85, mBgAlpha};
    }
    stringTex = mUiFontSmall->renderText(sdl->renderer(), string, color);
    dst.x += dst.w;
    dst.w = w;
    SDL_RenderCopy(sdl->renderer(), stringTex.get(), nullptr, &dst);

    mUiFontSmall->querySize(" Hz", &w, &h);
    color = {149, 149, 145, 255};
    stringTex = mUiFontSmall->renderText(sdl->renderer(), " Hz", color);
    dst.x += dst.w;
    dst.w = w;
    SDL_RenderCopy(sdl->renderer(), stringTex.get(), nullptr, &dst);
}

bool App::Context::shouldContinue() const
{
    return mContinue.load();
}

void App::Context::handleKeydownEvent(const SDL_Keycode key)
{
    if (key == SDLK_UP) {
        mPitchLimit += 2;
    }
    else if (key == SDLK_DOWN) {
        mPitchLimit -= 2;
    }
}

void App::Context::handleKeyupEvent(const SDL_Keycode key)
{
    if (key == SDLK_m) {
        if (mPitchLimitMode == PITCHLIMIT_MIN) {
            mPitchLimitMode = PITCHLIMIT_MAX;
        }
        else if (mPitchLimitMode == PITCHLIMIT_MAX) {
            mPitchLimitMode = PITCHLIMIT_MIN;
        }
    }
}

void App::Context::handleMousewheelEvent(const int y)
{
    mPitchLimit += 2 * y;
}
