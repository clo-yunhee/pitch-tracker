#include "processors.h"

ArrayXf Processors::filterFIR(const ArrayXf &a, const ArrayXf &x)
{
    const int n(x.size()), m(a.size());
    
    ArrayXf y(n);

    for (int k = 0; k < n; ++k) {
        double v = 0.0;
        for (int i = 0; i < m; ++i) {
            if (k - i >= 0 && k - i < n)
                v += (double) a(i) * (double) x(k - i);
        }
        y(k) = v;
    }

    return y;
}


