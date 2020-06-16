#ifndef TRACKER_CONTEXT_H
#define TRACKER_CONTEXT_H

#include <SDL2/SDL.h>
#include <Eigen/Dense>
#include "../sdl2/sdl2.h"
#include <atomic>
#include <thread>
#include <mutex>

namespace Tracker
{
    class Context : public SDL2::AudioListener
    {
    public:
        Context(int sampleRate);
        ~Context();

        virtual void onNewAudio(const Eigen::ArrayXf &data);

        float pitch();

    private:
        void performAnalysis(const Eigen::ArrayXf &data);

        int mSampleRate;
        
        std::atomic<bool> mContinue;
        std::thread mThread;

        Eigen::ArrayXf mData;
        std::mutex mDataLock;

        std::atomic<float> mPitch;
    };
}

#endif // TRACKER_CONTEXT_H
