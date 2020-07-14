#include "context.h"
#include <iostream>
#ifdef __ANDROID__
#   include <jni.h>
#endif
#ifdef _WIN32
#   include <windows.h>
#   include <shellscalingapi.h>
#endif

SDL2::Context::Context(Uint32 initFlags)
    : mWindow(nullptr), mRenderer(nullptr), mHaptic(nullptr)
{
    if (SDL_Init(initFlags) != 0) {
        std::cerr << "SDL_Init() error: " << SDL_GetError() << std::endl;
        exit(EXIT_FAILURE);
    }

    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1");
    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "2");

#ifdef _WIN32
    void* userDLL;
    BOOL(WINAPI *SetProcessDPIAware)(void); // Vista and later
    void* shcoreDLL;
    HRESULT(WINAPI *SetProcessDpiAwareness)(PROCESS_DPI_AWARENESS dpiAwareness); // Windows 8.1 and later

    userDLL = SDL_LoadObject("USER32.DLL");
    if (userDLL) {
        SetProcessDPIAware = (BOOL(WINAPI *)(void)) SDL_LoadFunction(userDLL, "SetProcessDPIAware");
    }

    shcoreDLL = SDL_LoadObject("SHCORE.DLL");
    if (shcoreDLL) {
        SetProcessDpiAwareness = (HRESULT(WINAPI *)(PROCESS_DPI_AWARENESS)) SDL_LoadFunction(shcoreDLL, "SetProcessDpiAwareness");
    }

    if (SetProcessDpiAwareness) {
        /* Try Windows 8.1+ version */
        HRESULT result = SetProcessDpiAwareness(PROCESS_PER_MONITOR_DPI_AWARE);
        SDL_Log("result: %x (S_OK = %x, E_INVALIDARG = %x, E_ACCESSDENIED = %x", result, S_OK, E_INVALIDARG, E_ACCESSDENIED);
        
        SDL_Log("called SetProcessDpiAwareness: %d", (result == S_OK) ? 1 : 0);
    }
    else if (SetProcessDPIAware) {
        /* Try Vista - Windows 8 version.
        This has a constant scale factor for all monitors.
        */
        BOOL success = SetProcessDPIAware();
        SDL_Log("called SetProcessDPIAware: %d", (int)success);
    }
#endif
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
            std::cerr << "SDL_HapticRumbleSupported() or SDL_HapticRumbleInit() error: " << SDL_GetError() << std::endl;
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

void SDL2::Context::rumblePlay(Uint32 length)
{
#ifdef __ANDROID__
    JNIEnv *env = (JNIEnv *) SDL_AndroidGetJNIEnv();
    jobject activity = (jobject) SDL_AndroidGetActivity();
    jclass clazz(env->GetObjectClass(activity));
    jmethodID methodId = env->GetMethodID(clazz, "vibeStart", "(J)V");
    env->CallVoidMethod(activity, methodId, (jlong) length);
    env->DeleteLocalRef(activity);
    env->DeleteLocalRef(clazz);
#else
    if (mHaptic != nullptr) {
        SDL_HapticRumblePlay(mHaptic, 0.25, length);
    }
#endif
}

void SDL2::Context::rumbleStop()
{
#ifdef __ANDROID__
    JNIEnv *env = (JNIEnv *) SDL_AndroidGetJNIEnv();
    jobject activity = (jobject) SDL_AndroidGetActivity();
    jclass clazz(env->GetObjectClass(activity));
    jmethodID methodId = env->GetMethodID(clazz, "vibeStop", "()V");
    env->CallVoidMethod(activity, methodId);
    env->DeleteLocalRef(activity);
    env->DeleteLocalRef(clazz);
#else
    if (mHaptic != nullptr) {
        SDL_HapticRumbleStop(mHaptic);
    }
#endif
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
    if (SDL_GetDisplayDPI(index, nullptr, &dpi, nullptr) != 0) {
        std::cerr << "SDL_GetDisplayDPI() error: " << SDL_GetError() << std::endl;
        exit(EXIT_FAILURE);
    }

    return dpi;
}

void SDL2::Context::windowSize(int *w, int *h)
{
    SDL_GetWindowSize(mWindow, w, h);
}
