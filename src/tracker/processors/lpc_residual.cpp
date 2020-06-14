#include "processors.h"

using Processors::filterFIR;
using Processors::lpc;
using Processors::hanning;

ArrayXf Processors::lpcResidual(const ArrayXf &x, int L, int shift, int order)
{
    const int len(x.size());

    ArrayXf win = hanning(L);
    ArrayXd res = ArrayXd::Zero(len);

    int start = 0;
    while (start + L < len) {
        ArrayXf xseg = x.segment(start, L) * win;

        ArrayXf a = lpc(xseg, order);
        
        ArrayXd inv = filterFIR(a, xseg).cast<double>();
        inv *= sqrt(xseg.square().sum() / inv.square().sum());

        res.segment(start, L) += inv;

        start += shift;
    }

    return res.cast<float>();
}
