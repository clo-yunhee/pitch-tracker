#include "audio.h"
#include <iostream>

using namespace Eigen;

SDL2::Audio::Audio(int sampleRate)
    : mSampleRate(sampleRate), mInternalBufferSize(1024)
{
    SDL_AudioSpec want, have;
    SDL_zero(want);
    want.freq = sampleRate;
    want.format = AUDIO_F32;
    want.channels = 1;
    want.samples = mInternalBufferSize;
    want.callback = SDL2::Audio::callback;
    want.userdata = this;

    mDeviceId = SDL_OpenAudioDevice(nullptr, true, &want, &have, 0);

    if (mDeviceId <= 0) {
        std::cerr << "SDL_OpenAudioDevice() error: " << SDL_GetError() << std::endl;
        exit(EXIT_FAILURE);
    }

    mInternalBufferSize = have.samples;
    mSpec = std::move(have);
}

SDL2::Audio::~Audio()
{
    close();
}

int SDL2::Audio::sampleRate() const
{
    return mSampleRate;
}

void SDL2::Audio::resume()
{
    SDL_PauseAudioDevice(mDeviceId, false);
}

void SDL2::Audio::pause()
{
    SDL_PauseAudioDevice(mDeviceId, true);
}

void SDL2::Audio::toggle()
{
    switch (SDL_GetAudioDeviceStatus(mDeviceId)) {
    case SDL_AUDIO_PLAYING:
        pause();
        break;
    case SDL_AUDIO_PAUSED:
        resume();
        break;
    default:
        break;
    }
}

void SDL2::Audio::lock()
{
    SDL_LockAudioDevice(mDeviceId);
}

void SDL2::Audio::unlock()
{
    SDL_UnlockAudioDevice(mDeviceId);
}

void SDL2::Audio::close()
{
    SDL_CloseAudioDevice(mDeviceId);
}

void SDL2::Audio::addListener(AudioListener *listener)
{
    mListeners.push_back(listener);
}

void SDL2::Audio::callback(void *userdata, Uint8 *stream, int len)
{
    auto self = static_cast<SDL2::Audio *>(userdata);
    auto data = reinterpret_cast<const float *>(stream);
    
    int nsamp = len / sizeof(float);

    ArrayXf array(nsamp);
    memcpy(array.data(), data, len);

    for (AudioListener *listener : self->mListeners) {
        listener->onNewAudio(array);
    }
}

