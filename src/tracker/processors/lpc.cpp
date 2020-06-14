#include "processors.h"

ArrayXf Processors::lpc(const ArrayXf &x, int order)
{
    const int n = x.size(), m = order;
    
    ArrayXd a = ArrayXd::Zero(m + 1);
    a(0) = 1.0;

    ArrayXd b1 = ArrayXd::Zero(n + 1);
    ArrayXd b2 = ArrayXd::Zero(n + 1);
    ArrayXd aa = ArrayXd::Zero(m + 1);

    double p = 0.0;
    for (int j = 0; j < n; ++j)
        p += (double) x(j) * (double) x(j);

    double xms = p / static_cast<double>(n);
    if (xms <= 0.0)
        goto done;

    b1(1) = x(0);
    b2(n - 1) = x(n - 1);
    for (int j = 2; j <= n; ++j)
        b1(j) = b2(j - 1) = x(j - 1);

    for (int i = 1; i <= m; ++i) {
        double num = 0.0, denom = 0.0;
        for (int j = 1; j <= n - i; ++j) {
            num += b1(j) * b2(j);
            denom += b1(j) * b1(j) + b2(j) * b2(j);
        }

        if (denom <= 0.0)
            goto done;

        a(i) = (2.0 * num) / denom;

        xms *= 1.0 - a(i) * a(i);

        for (int j = 1; j <= i - 1; ++j)
            a(j - 1) = aa(j) - a(i) * aa(i - j);

        if (i < m) {
            for (int j = 1; j <= i; ++j)
                aa(j) = a(j);
            for (int j = 1; j <= n - i - 1; ++j) {
                b1(j) -= aa(i) * b2(j);
                b2(j) = b2(j + 1) - aa(i) * b1(j + 1);
            }
        }
    }

done:
    return -a.cast<float>();
}
