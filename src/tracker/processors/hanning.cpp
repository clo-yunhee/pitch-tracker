#include "processors.h"

ArrayXf Processors::hanning(int L)
{
    ArrayXf w(L);
    for (int k = 0; k < L; ++k) {
        w(k) = (0.5 - 0.5 * cos((2.0 * M_PI * k) / (double) (L - 1)));
    }
    return w;
}
