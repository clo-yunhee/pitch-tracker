#include "processors.h"

float Processors::pitchAMDF_M(const ArrayXf &x, int sampleRate, float minimumPitch, float maximumPitch, float alpha, float ratio, float sensitivity)
{
    const int maxShift = x.size();

    const int maxPeriod = std::min<float>(ceil(sampleRate / minimumPitch), maxShift - 1);
    const int minPeriod = std::max<float>(floor(sampleRate / maximumPitch), 2);

    // Calculate the autocorrelation function of the converted one-bit signal.

    ArrayXf ac(maxShift);

    for (int i = 0; i < maxShift; ++i) {
        const auto frame1 = x.segment(0, maxShift - i);
        const auto frame2 = x.segment(i, maxShift - i);

        double v = 0.0;
        for (int n = 0; n < maxShift - i; ++n) {
            v += (amd(n) == 1.0F && amd(i + n) == 1.0F) ? 1.0 : 0.0;
        }
        ac(i) = v;
    }

    // Find the local peak.

    const auto acSub = ac.segment(minPeriod, maxPeriod - minPeriod + 1);
    float acMax = acSub.maxCoeff();
    float acMin = acSub.minCoeff();
    
    const float cutoff = round((sensitivity * (acMax - acMin)) + acMin);

    int j = minPeriod;
    while (j <= maxPeriod && ac(j) < cutoff) {
        j++;
    }
    
    const int searchLength = minPeriod / 2;

    int posMax = j;
    acMax = ac(j);

    int i = j;
    while (i < j + searchLength && i <= maxPeriod) {
        i++;

        float cur = ac(i);
        if (cur > acMax) {
            posMax = i;
            acMax = cur;
        }
    }
   
    if (round(acMax * ratio) > acMin) {
        return (float) sampleRate / (float) posMax;
    }
    else {
        return 0.0F;
    }
}

