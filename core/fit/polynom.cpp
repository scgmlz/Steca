#include "core/fit/polynom.h"

void Polynom::setY(const double* P, const int nXY, const double* X, double* Y) const
{
    for (int i=0 ; i<nXY; ++i) {
        double ret = 0;
        double xPow = 1;
        for (int ip=0; ip<nPar_; ++ip) {
            ret += P[ip] * xPow;
            xPow *= *(X+i);
        }
        Y[i] = ret;  // <--- return values are set here
    }
}

void Polynom::setDY(const double*, const int nXY, const double* X, double* Jacobian) const
{
    for (int i=0; i<nXY; ++i) {
        double xPow = 1;
        for (int ip=0; ip<nPar_; ++ip) {
            *Jacobian++ = xPow;  // <--- return values are set here
            xPow *= *(X+i);
        }
    }
}
