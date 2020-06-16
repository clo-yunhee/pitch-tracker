#include "context.h"
#include <iostream>
#include <iomanip>

using namespace Eigen;

Tracker::Context::Context(int sampleRate)
    : mSampleRate(sampleRate), mContinue(true)
{
    mData.setZero(512);

    mThread = std::thread([this] {
            float avgDur = -1;
            while (mContinue.load()) { 
                mDataLock.lock();
                ArrayXf data = mData.eval();
                mDataLock.unlock();

                // Count how long one analysis cycle takes.
                Uint32 start = SDL_GetTicks();
                
                performAnalysis(data);

                Uint32 dur = SDL_GetTicks() - start;

                // Subtract that duration from the sleep duration.
                // If it results in negative sleep time, don't sleep.
                if (avgDur < 0) {
                    avgDur = dur;
                }
                else {
                    avgDur = (float) (avgDur + dur) / 2.0F;
                }
                std::cout << "Analysis taking " << std::fixed << std::setw(3) << std::setprecision(2)
                          << avgDur << " milliseconds.\r" << std::flush;

                if (dur < 30) {
                    SDL_Delay(30 - dur);
                }
            }
            std::cout << std::endl;
        });
}

Tracker::Context::~Context()
{
    mContinue.store(false);
    mThread.join();
}

void Tracker::Context::onNewAudio(const ArrayXf &data)
{
    mDataLock.lock();
    mData = data;
    mDataLock.unlock();
}

float Tracker::Context::pitch()
{
    return mPitch.load();
}
