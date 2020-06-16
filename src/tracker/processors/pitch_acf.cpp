#include "processors.h"

using Processors::hanning;

float Processors::pitchACF(const ArrayXf &x, int sampleRate, float minimumPitch, float maximumPitch, float cutoff)
{
    const int maxShift = x.size();

    const int maxPeriod = std::min<float>(ceil(sampleRate / minimumPitch), maxShift - 1);
    const int minPeriod = std::max<float>(floor(sampleRate / maximumPitch), 2);

    // Window the signal with Hanning function.

    ArrayXf w = hanning(maxShift);

    // Calculate the autocorrelation function of thei signal.

    ArrayXf ac(maxShift);

    for (int i = 0; i < maxShift; ++i) {
        ac(i) = (w.segment(0, maxShift - i) * x.segment(0, maxShift - i)
                    * w.segment(i, maxShift - i) * x.segment(i, maxShift - i)).sum()
                / (w.segment(0, maxShift - i) * w.segment(i, maxShift - i)).sum();
    }

    // Find the local peak.

    std::vector<int> maxPositions = pickPeaks(ac);

    if (maxPositions.empty()) {
        return 0.0F;
    }

    const double actualCutoff = cutoff * ac(0);
    float pitch = 0.0F;

    for (int i : maxPositions) {
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

