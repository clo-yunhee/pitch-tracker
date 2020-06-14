#ifndef SDL2_AUDIO_H
#define SDL2_AUDIO_H

#include <SDL2/SDL.h>
#include <Eigen/Dense>
#include <mutex>
#include <vector>

namespace SDL2
{
    class AudioListener
    {
    public:
        virtual void onNewAudio(const Eigen::ArrayXf &data) = 0;
    };

    class Audio
    {
    public:
        Audio(int sampleRate);
        ~Audio();

        int sampleRate() const;

        void resume();
        void pause();
        void toggle();
        
        void lock();
        void unlock();
        
        void close();

        void addListener(AudioListener *listener);

    private:
        static void callback(void *userdata, Uint8 *stream, int len);
        
        SDL_AudioSpec mSpec;
        SDL_AudioDeviceID mDeviceId;

        int mSampleRate;
        int mInternalBufferSize;

        std::mutex mLock;

        std::vector<AudioListener *> mListeners;
    };
}

#endif // SDL2_AUDIO_H
