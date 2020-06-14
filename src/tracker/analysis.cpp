#include "context.h"
#include "processors/processors.h"

using namespace Eigen;

void Tracker::Context::performAnalysis(const ArrayXf &data)
{
    float pitch = Processors::pitchAMDF_M(data, mSampleRate, 75, 1000, 0.4, 5.0, 0.1);
    
    if (pitch) {
        std::cout << "Pitch: " << round(pitch) << " Hz" << std::endl;
    }
    else {
        std::cout << "U." << std::endl;
    }
}
