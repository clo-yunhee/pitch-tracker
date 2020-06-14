#include "processors.h"

ArrayXf Processors::filterIIR(const ArrayXf &b, const ArrayXf &a, const ArrayXf &x)
{
    const int n(x.size()), m1(b.size()), m2(a.size());
    
    ArrayXf y(n);

    for (int k = 0; k < n; ++k) {
        double v = 0.0;
        for (int i = 0; i < m1; ++i) {
            if (k - i >= 0 && k - i < n)
                v += (double) b(i) * (double) x(k - i);
        }
        for (int j = 1; j < m2; ++j) {
            if (k - j >= 0 && k - j < n)
                v -= (double) a(j) * (double) y(k - j);
        }
        y(k) = v / (double) a(0);
    }

    return y;
}

