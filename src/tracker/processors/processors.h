#ifndef TRACKER_PROCESSORS_PROCESSORS_H
#define TRACKER_PROCESSORS_PROCESSORS_H

#include <Eigen/Dense>
#include <cmath>
#include <iostream>
#include <numeric>

using namespace Eigen;

namespace Processors
{
    ArrayXf filterFIR(const ArrayXf &a, const ArrayXf &x);
   
    ArrayXf filterIIR(const ArrayXf &b, const ArrayXf &a, const ArrayXf &x);

    ArrayXf lpc(const ArrayXf &x, int order);
    
    ArrayXf lpcResidual(const ArrayXf &x, int L, int shift, int order);

    ArrayXf hanning(int L);

    float pitchAMDF_M(const ArrayXf &x, int sampleRate, float minimumPitch, float maximumPitch, float alpha, float ratio, float sensitivity);
}

#endif // TRACKER_PROCESSORS_PROCESSORS_H
