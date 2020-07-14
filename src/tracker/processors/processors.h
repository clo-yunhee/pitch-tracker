#ifndef TRACKER_PROCESSORS_PROCESSORS_H
#define TRACKER_PROCESSORS_PROCESSORS_H

#include <Eigen/Dense>
#include <cmath>
#include <iostream>
#include <numeric>
#include <vector>

#undef M_PI
#define M_PI 3.14159265358979323846264338327950288

using namespace Eigen;

namespace Processors
{
    ArrayXf filterFIR(const ArrayXf &a, const ArrayXf &x);
   
    ArrayXf filterIIR(const ArrayXf &b, const ArrayXf &a, const ArrayXf &x);

    ArrayXf lpc(const ArrayXf &x, int order);
    
    ArrayXf lpcResidual(const ArrayXf &x, int L, int shift, int order);

    ArrayXf hanning(int L);

    float pitchACF(const ArrayXf &x, int sampleRate, float minimumPitch, float maximumPitch, float cutoff);

    float pitchAMDF_M(const ArrayXf &x, int sampleRate, float minimumPitch, float maximumPitch, float alpha);

    std::vector<int> pickPeaks(Ref<const ArrayXf> x);
}

#endif // TRACKER_PROCESSORS_PROCESSORS_H
