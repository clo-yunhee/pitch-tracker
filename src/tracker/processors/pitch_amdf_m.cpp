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

    std::vector<uint32_t> amd1b(maxShift / 32 + 1); 
    
    for (int ind = 0; ind < maxShift / 32 + 1; ++ind) {
        uint32_t v = 0U;

        for (int bit = 0; bit < 32; ++bit) {
            int i = ind * 32 + bit;
            if (i < maxShift && amd(i) <= theta)
                v |= 1U << bit;
        }

        amd1b[ind] = v;
    }

    // Calculate the autocorrelation function of the one-bit signal.

    ArrayXf ac(maxShift);

    for (int i = 0; i < maxShift; ++i) {
        int v = 0;
        for (int j = 0; j < maxShift - i; ++j) {
            int ind1 = j / 32;
            int bit1 = j % 32;

            int ind2 = (i + j) / 32;
            int bit2 = (i + j) % 32;

            if (amd1b[ind1] & (1U << bit1) && amd1b[ind2] & (1U << bit2))
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

