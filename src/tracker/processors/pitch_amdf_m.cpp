#include "processors.h"

using Processors::hanning;

float Processors::pitchAMDF_M(const ArrayXf &x, int sampleRate, float minimumPitch, float maximumPitch, float alpha)
{
    const int maxShift = x.size();

    const int maxPeriod = std::min<float>(ceil(sampleRate / minimumPitch), maxShift - 1);
    const int minPeriod = std::max<float>(floor(sampleRate / maximumPitch), 2);

    // Calculate the absolute magnitude difference function of the signal.

    ArrayXf amd(maxShift);

    for (int i = 0; i < maxShift; ++i) {
        amd(i) = (x.segment(0, maxShift - i) - x.segment(i, maxShift - i)).abs().sum() / (float) (maxShift - i);
    }

    // Convert to one-bit signal.

    const double Vmax = amd.maxCoeff();
    const double Vmin = amd.minCoeff();
    const double theta = alpha * (Vmax + Vmin);

    std::vector<bool> amd1b(maxShift);

    for (int i = 0; i < maxShift; ++i) {
        if (amd(i) <= theta)
            amd1b[i] = true;
        else
            amd1b[i] = false;
    }

    // Calculate the autocorrelation function of the one-bit signal.

    ArrayXf ac(maxShift);

    for (int i = 0; i < maxShift; ++i) {
        int v = 0;
        for (int j = 0; j < maxShift - i; ++j) {
            if (amd1b[j] && amd1b[i + j])
                v++;
        }
        ac(i) = v;
    }

    // Find the global peak.

    std::vector<int> maxPositions = pickPeaks(ac.segment(minPeriod, maxPeriod - minPeriod + 1));

    if (maxPositions.empty()) {
        return 0.0F;
    }

    const double actualCutoff = alpha * ac(0);
    float pitch = 0.0F;

    for (auto it = maxPositions.cbegin(); it != maxPositions.cend(); ++it) {
        int i = minPeriod + *it;
        if (ac(i) >= actualCutoff) {
            pitch = (float) sampleRate / (float) i;

            if (pitch < minimumPitch)
                continue;
            else
                break;
        }
    }

    if (pitch < minimumPitch) {
        return 0.0F;
    }
    else {
        return pitch;
    }
}

