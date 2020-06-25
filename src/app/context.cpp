#include "app.h"
#include "../sdl2/sdl2.h"
#include "../tracker/tracker.h"
#include <iostream>

App::Context::Context(SDL2::Context *sdl, int sampleRate)
    : mContinue(true),
      mPaused(false),
      mAudio(new SDL2::Audio(sampleRate)),
      mTrackerContext(new Tracker::Context(sampleRate)),
      mUiFontBig(new SDL2::Font("Montserrat.ttf", sdl->dipSizeInPixels(32))),
      mUiFontMedium(new SDL2::Font("Montserrat.ttf", sdl->dipSizeInPixels(24))),
      mUiFontSmall(new SDL2::Font("Montserrat.ttf", sdl->dipSizeInPixels(18))),
      mUiFontTiny(new SDL2::Font("Montserrat.ttf", sdl->dipSizeInPixels(14))),
      mPitchLimit(170),
      mPitchLimitMode(PITCHLIMIT_MIN),
      mShowGraph(true),
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

void App::Context::handleEvent(SDL2::Context *sdl, const SDL_Event *ev)
{
    if (ev->type == SDL_QUIT
            || (ev->type == SDL_KEYDOWN && ev->key.keysym.sym == SDLK_ESCAPE)) {
        mContinue.store(false);
        return;
    }

    if (ev->type == SDL_APP_WILLENTERBACKGROUND || ev->type == SDL_APP_TERMINATING) {
        mPaused.store(true);
    }
    else if (ev->type == SDL_APP_DIDENTERFOREGROUND) {
        mPaused.store(false);
    }

    // Rumble if pitch is outside limits.
    float pitch = mTrackerContext->pitch();

    if  (pitch != 0 &&
            ((mPitchLimitMode == PITCHLIMIT_MIN && pitch <= mPitchLimit)
                || (mPitchLimitMode == PITCHLIMIT_MAX && pitch >= mPitchLimit))) {
        sdl->rumblePlay(0.8, 100);
    }

    if (mPaused.load()) {
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
    case SDL_FINGERMOTION:
        if (ev->tfinger.pressure >= 0.125) {
            handleFingermotionEvent(sdl, ev->tfinger.dx, ev->tfinger.dy);
        }
        break;
    case SDL_FINGERDOWN:
        handleFingerdownEvent(ev->tfinger.timestamp, ev->tfinger.x, ev->tfinger.y);
        break;
    case SDL_FINGERUP:
        handleFingerupEvent(ev->tfinger.timestamp, ev->tfinger.x, ev->tfinger.y);
        break;
    default:
        break;
    }
}

void App::Context::renderApp(SDL2::Context *sdl)
{
    int targetWidth, targetHeight;
    sdl->windowSize(&targetWidth, &targetHeight);

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
        color = {255, 85, 85, Uint8(mBgAlpha / 32)};
        dst = {0, 0, targetWidth, targetHeight};
        SDL_SetRenderDrawBlendMode(sdl->renderer(), SDL_BLENDMODE_BLEND);
        SDL_SetRenderDrawColor(sdl->renderer(), color.r, color.g, color.b, color.a);
        SDL_RenderFillRect(sdl->renderer(), &dst);
    }

    // Help text.
    SDL_Color helpTextLeftColor = {149, 149, 145, 255};
    SDL_Color helpTextRightColor = {255, 121, 198, 255};
    std::vector<const char *> helpText{
        "Floor/ceiling: ",
            "M / Touch top half",
        "Limit up: ",
            "Up / Scroll up",
        "Limit down: ",
            "Down / scroll down",
        "Tracer on/off: ",
            "T / Touch bottom half",
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
        snprintf(string, maxLen, "%d", (int) pitch);
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

    int y2 = dst.y + 25 + mUiFontSmall->lineSkip();

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
        dst.y = y2;
        SDL_RenderCopy(sdl->renderer(), stringTex.get(), nullptr, &dst);
    }

    y2 += mUiFontSmall->height();

    // Pitch graph.
    if (mShowGraph) {
        color = {129, 128, 232, 192};
        int wg = std::min(sdl->dipSizeInPixels(320), (targetWidth * 2) / 3);
        int hg = sdl->dipSizeInPixels(80);
        int xg = targetWidth / 2 - wg / 2;
        int yg = y2 + 30;
        dst = {xg, yg, wg, hg};
        SDL_SetRenderDrawBlendMode(sdl->renderer(), SDL_BLENDMODE_BLEND);
        SDL_SetRenderDrawColor(sdl->renderer(), color.r, color.g, color.b, color.a);
        SDL_RenderFillRect(sdl->renderer(), &dst);

        color = {248, 248, 242, 255};
        SDL_SetRenderDrawColor(sdl->renderer(), color.r, color.g, color.b, color.a);
        
        auto pitches = mTrackerContext->pitches();

        int minSpurtLength = 4;

        // Remove pitch spurts shorter than 4 frames.
        int startIndex = 0;
        for (int i = 0; i < pitches.size(); ++i) {
            float fi = pitches[i];

            if (fi == 0) {
                if (i - startIndex <= minSpurtLength) {
                    for (int j = startIndex; j < i; ++j) {
                        pitches[i] = 0;
                    }
                }

                startIndex = i;
            }
        }

        // Min / max pitches.
        float minPitch = INT_MAX, maxPitch = 0;

        for (int i = 0; i < pitches.size(); ++i) {
            float fi = pitches[i];
            if (fi != 0) {
                minPitch = std::min(minPitch, fi);
                maxPitch = std::max(maxPitch, fi);
            }
        }

        if (minPitch > maxPitch) {
            minPitch = -2;
            maxPitch = -1;
        }

        struct PointF { float x, y; };
        std::vector<PointF> path;

        float scale = 4;
        float thickness = 3;

        for (int i = 0; i < pitches.size(); ++i) {
            float fi = pitches[i];

            if ((fi == 0 || i == (pitches.size() - 1)) && path.size() > 0) {
                SDL_RenderSetScale(sdl->renderer(), 1.0F / scale, 1.0F / scale);
                for (int j = 0; j < path.size() - 1; ++j) {
                    thickLineRGBA(
                            sdl->renderer(),
                            path[j].x * scale, path[j].y * scale,
                            path[j + 1].x * scale, path[j + 1].y * scale,
                            thickness * scale,
                            color.r, color.g, color.b, color.a
                    );
                }
                SDL_RenderSetScale(sdl->renderer(), 1, 1);

                path.clear();
            }
        
            if (fi != 0) {
                float cx = (wg * (i + 0.5F)) / (float) pitches.size();
                float cy = (float) (hg * (maxPitch - fi)) / (float) (maxPitch - minPitch);
                
                path.push_back(PointF{xg + cx, yg + cy});
            }
        }

        // Find what marker step makes less than 5 markers.

        if (minPitch > 0) {
            int yMinPitch = yg + hg;
            int yMaxPitch = yg;
           
            if ((int) minPitch == mPitchLimit) {
                color = {255, 85, 85, 255};
            }
            else {
                color = {129, 128, 232, 192};
            }
            snprintf(string, maxLen, "%d", (int) minPitch);
            mUiFontTiny->querySize(string, &dst.w, &dst.h);
            dst.x = xg - 4 - dst.w;
            dst.y = yMinPitch - dst.h / 2;
            stringTex = mUiFontTiny->renderText(sdl->renderer(), string, color);
            SDL_RenderCopy(sdl->renderer(), stringTex.get(), nullptr, &dst);

            if ((int) maxPitch == mPitchLimit) {
                color = {255, 85, 85, 255};
            }
            else {
                color = {129, 128, 232, 192};
            }
            snprintf(string, maxLen, "%d", (int) maxPitch);
            mUiFontTiny->querySize(string, &dst.w, &dst.h);
            dst.x = xg - 4 - dst.w;
            dst.y = yMaxPitch - dst.h / 2;
            stringTex = mUiFontTiny->renderText(sdl->renderer(), string, color);
            SDL_RenderCopy(sdl->renderer(), stringTex.get(), nullptr, &dst);

            if (minPitch < mPitchLimit && mPitchLimit < maxPitch) {
                int yLimit = yg + hg * ((maxPitch - mPitchLimit) / (maxPitch - minPitch));

                color = {255, 85, 85, 255};
                snprintf(string, maxLen, "%d", (int) mPitchLimit);
                mUiFontTiny->querySize(string, &dst.w, &dst.h);
                dst.x = xg - 4 - dst.w;
                dst.y = yLimit - dst.h / 2;
                stringTex = mUiFontTiny->renderText(sdl->renderer(), string, color);
                SDL_RenderCopy(sdl->renderer(), stringTex.get(), nullptr, &dst);

                thickLineRGBA(sdl->renderer(), xg, yLimit, xg + wg, yLimit, thickness, color.r, color.g, color.b, color.a / 2);
            }
        }
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

bool App::Context::isPaused() const
{
    return mPaused.load();
}

void App::Context::handleKeydownEvent(const SDL_Keycode key)
{
    if (key == SDLK_UP) {
        mPitchLimit += 2;
    }
    else if (key == SDLK_DOWN) {
        mPitchLimit -= 2;
    }
    mPitchLimit = std::clamp(mPitchLimit, 60, 1400);
}

void App::Context::handleKeyupEvent(const SDL_Keycode key)
{
    if (key == SDLK_m) {
        togglePitchLimitMode();
    }
    else if (key == SDLK_t) {
        togglePitchGraph();
    }
}

void App::Context::handleMousewheelEvent(const int y)
{
    setPitchLimit(mPitchLimit + 2 * y);
}

void App::Context::handleFingermotionEvent(SDL2::Context *sdl, const float dx, const float dy)
{
    int width, height;
    float dpi = sdl->windowDisplayDpi();
    sdl->windowSize(&width, &height);

    int dxUnits = (width * dx * 140) / (2 * dpi);
    int dyUnits = (height * dy * 140) / (2 * dpi);

    setPitchLimit(mPitchLimit - dyUnits);

    mTouchAbsDx += abs(dxUnits);
    mTouchAbsDy += abs(dyUnits);
}

void App::Context::handleFingerdownEvent(const Uint32 time, const float x, const float y)
{
    mTouchTime = time;
    mTouchAbsDx = 0;
    mTouchAbsDy = 0;
}

void App::Context::handleFingerupEvent(const Uint32 time, const float x, const float y)
{
    if (time - mTouchTime < 500 && mTouchAbsDx < 16 && mTouchAbsDy < 16) {
        // Top half of the screen.
        if (y <= 0.5) {
            togglePitchLimitMode();
        }
        else {
            togglePitchGraph();
        }
    }
}

void App::Context::setPitchLimit(const int newPitchLimit)
{
    mPitchLimit = std::clamp(newPitchLimit, 60, 1400);
}

void App::Context::togglePitchLimitMode()
{
    if (mPitchLimitMode == PITCHLIMIT_MIN) {
        mPitchLimitMode = PITCHLIMIT_MAX;
    }
    else if (mPitchLimitMode == PITCHLIMIT_MAX) {
        mPitchLimitMode = PITCHLIMIT_MIN;
    }
}

void App::Context::togglePitchGraph()
{
    mShowGraph = !mShowGraph;
}
