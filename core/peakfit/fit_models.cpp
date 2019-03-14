//  ***********************************************************************************************
//
//  Steca: stress and texture calculator
//
//! @file      core/peakfit/fit_models.cpp
//! @brief     Implements fit models
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
//  ***********************************************************************************************

#include "core/peakfit/fit_models.h"
#include "core/typ/curve.h"
#include "core/fitengine/double_with_error.h"
#include "core/fitengine/fit_wrapper.h"
#include "core/peakfit/outcome.h"
#include <cerf.h>
#include <qmath.h>
#define SQR(x) ((x)*(x))

//! A Fwhm finder as a fit function. avoids reimplementing

class FindFwhm : public FitFunction {
public:
    static DoubleWithError fromFitted(const Fitted& F);
    void setY(const double* P, const int nXY, const double* X, double* Y) const final;
    void setDY(const double* P, const int nXY, const double* X, double* Jacobian) const final;
    int nPar() const final { return 1; }
private:
    FindFwhm(const Fitted& fitted) : fitted_{fitted} { }
    double getY(double x, const double *P) const;
    const Fitted& fitted_;
};


//  ***********************************************************************************************
//! @class Gaussian

namespace {
const double prefac = 1 / sqrt(2*M_PI);
}

void Gaussian::setY(const double* P, const int nXY, const double* X, double* Y) const
{
    double center = P[0];
    double stdv   = P[1] / sqrt(8*log(2));
    double inten  = P[2];
    for (int i=0 ; i<nXY; ++i)
        Y[i] = inten*prefac/stdv*exp(-SQR(*(X+i)-center)/(2*SQR(stdv)));
}

void Gaussian::setDY(const double* P, const int nXY, const double* X, double* Jacobian) const
{
    double center = P[0];
    double stdv   = P[1] / sqrt(8*log(2));
    double inten  = P[2];
    for (int i=0; i<nXY; ++i) {
        double dx = *(X+i) - center;
        double g = prefac/stdv*exp(-SQR(dx)/(2*SQR(stdv)));
        *Jacobian++ = inten*g*(dx)/SQR(stdv);
        *Jacobian++ = inten*g*(SQR((dx)/stdv)-1)/stdv;
        *Jacobian++ = g;
    }
}

//  ***********************************************************************************************
//! @class Lorentzian

void Lorentzian::setY(const double* P, const int nXY, const double* X, double* Y) const
{
    double center = P[0];
    double hwhm   = P[1]/2;
    double inten  = P[2];
    double ampl   = inten * hwhm / M_PI;
    for (int i=0 ; i<nXY; ++i)
        Y[i] = ampl/(SQR(*(X+i)-center)+SQR(hwhm));
}

void Lorentzian::setDY(const double* P, const int nXY, const double* X, double* Jacobian) const
{
    double center = P[0];
    double hwhm   = P[1]/2;
    double inten  = P[2];
    for (int i=0; i<nXY; ++i) {
        double dx = *(X+i) - center;
        double deno = SQR(dx)+SQR(hwhm);
        *Jacobian++ = inten*hwhm/M_PI * 2 * dx / SQR(deno);
        *Jacobian++ = inten/2/M_PI/deno * (1 - 2*SQR(hwhm)/deno);
        *Jacobian++ = hwhm/M_PI/deno;
    }
}

//  ***********************************************************************************************
//! @class Voigt

namespace {

//! approximates the parameter derivative for f. fxp0 = f(x, p0)
template <typename F>
inline void derivative(
    const F f, double fxp0, double x, const double *P, uint nPar, double* Jacobian)
{
    const double rho = 1e-3;
    double *params = const_cast<double*>(P);
    for (uint i = 0; i < nPar; ++i) {
        // get appropriate delta for param:
        double delta = rho*(fabs(params[i])+rho);
        delta = std::copysign(delta / (delta + 1), params[i]);

        double origParam = params[i];
        params[i] += delta;
        Jacobian[i] = (f(x, params) - fxp0) / delta;
        // make sure to retain const-ness of P:
        params[i] = origParam;
    }
    return;
}

double voigt_of_P(double x, const double *P) {
    return P[2] * voigt(x-P[0], P[1], P[1]*P[3]);
}

} // namespace

void Voigt::setY(const double *P, const int nXY, const double *X, double *Y) const
{
    for (int i=0 ; i<nXY; ++i)
        Y[i] = voigt_of_P(X[i], P);
}

void Voigt::setDY(const double* P, const int nXY, const double* X, double* Jacobian) const
{
    for (int i=0; i<nXY; ++i) {
        double base = voigt_of_P(X[i], P);
        derivative(&voigt_of_P, base, X[i], P, nPar(), Jacobian);
        Jacobian += nPar();
    }
}

Mapped Voigt::outcome(const Fitted& F) const
{
    double fwhm = FindFwhm::fromFitted(F).value();

    Mapped ret;
    ret["center"]          = F.parValAt(0);
    ret["sigma_center"]    = F.parErrAt(0);
    ret["intensity"]       = F.parValAt(1);
    ret["sigma_intensity"] = F.parErrAt(1);
    ret["fwhm"]            = fwhm;
    ret["sigma_fwhm"]      = fwhm / F.parValAt(1) * F.parErrAt(1);
    ret["gammaOverSigma"]       = F.parValAt(3);
    ret["sigma_gammaOverSigma"] = F.parErrAt(3);
    return ret;
}

//  ***********************************************************************************************
//! @class FindFwhm

double FindFwhm::getY(double x, const double *P) const
{
    return fitted_.y(x + *P*0.5);
}

void FindFwhm::setY(const double* P, const int nXY, const double* X, double* Y) const
{   // 'curve' has only one point.
    *Y = getY(*X, P);
}

void FindFwhm::setDY(const double* P, const int nXY, const double* X, double* Jacobian) const
{
    auto f = [this](double x, const double*P){ return getY(x, P); };  // why c++, why?
    derivative(f, f(*X, P), *X, P, nPar(), Jacobian);
}

DoubleWithError FindFwhm::fromFitted(const Fitted& F) {
    double p0 = F.parValAt(0);
    double ampl = F.y(p0);

    Curve curve;
    curve.append(p0, ampl/2.0);

    Fitted res = FitWrapper().execFit(new FindFwhm{F}, curve, {1});
    return {fabs(res.parValAt(0)), res.parErrAt(0)};
}
