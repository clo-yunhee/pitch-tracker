#include "context.h"
#include <iostream>

using namespace Eigen;

Tracker::Context::Context(int sampleRate)
    : mSampleRate(sampleRate), mContinue(true)
{
    mData.setZero(32);

    mThread = std::thread([this] {
            while (mContinue.load()) { 
                mDataLock.lock();
                ArrayXf data = mData.eval();
                mDataLock.unlock();

                // Count how long one analysis cycle takes.
                Uint32 start = SDL_GetTicks();
                
                performAnalysis(data);
    
                Uint32 end = SDL_GetTicks();

                // Subtract that duration from the sleep duration.
                // If it results in negative sleep time, don't sleep.
                std::cout << "Analysis took " << (end - start) << " milliseconds." << std::endl;

                SDL_Delay(100);
            }
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

