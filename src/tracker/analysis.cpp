#include "context.h"
#include "processors/processors.h"

using namespace Eigen;

void Tracker::Context::performAnalysis(const ArrayXf &data)
{
    float pitch = Processors::pitchAMDF_M(data, mSampleRate, 60, 1400, 0.4);

    mPitch.store(pitch);

    mPitchesLock.lock();
    mPitches.pop_front();
    mPitches.push_back(pitch);
    mPitchesLock.unlock();
}
