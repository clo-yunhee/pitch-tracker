#ifndef TRACKER_CONTEXT_H
#define TRACKER_CONTEXT_H

#include <SDL2/SDL.h>
#include <Eigen/Dense>
#include "../sdl2/sdl2.h"
#include <atomic>
#include <thread>
#include <mutex>
#include <list>
#include <vector>

namespace Tracker
{
    class Context : public SDL2::AudioListener
    {
    public:
        Context(int sampleRate);
        ~Context();

        virtual void onNewAudio(const Eigen::ArrayXf &data);

        float pitch();
        std::vector<float> pitches();

    private:
        void performAnalysis(const Eigen::ArrayXf &data);

        int mSampleRate;
        
        std::atomic<bool> mContinue;
        std::thread mThread;

        Eigen::ArrayXf mData;
        std::mutex mDataLock;

        std::atomic<float> mPitch;

        std::list<float> mPitches;
        std::mutex mPitchesLock;
    };
}

#endif // TRACKER_CONTEXT_H
