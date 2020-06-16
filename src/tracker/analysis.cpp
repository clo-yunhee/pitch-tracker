#include "context.h"
#include "processors/processors.h"

using namespace Eigen;

void Tracker::Context::performAnalysis(const ArrayXf &data)
{
    mPitch = Processors::pitchAMDF_M(data, mSampleRate, 60, 1400, 0.4);
}
