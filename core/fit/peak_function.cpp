//  ***********************************************************************************************
//
//  Steca: stress and texture calculator
//
//! @file      core/fit/peak_function.cpp
//! @brief     Implements class PeakFunction; defines and implements its subclasses
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
//  ***********************************************************************************************

#include "core/fit/peak_function.h"
#include "core/typ/curve.h"
#include "core/fit/fit_methods.h"
#include "core/fit/raw_outcome.h"
#include "qcr/base/debug.h" // ASSERT
#include "LevMar/LM/levmar.h"
// waiting for activation #include "lmfit/lib/lmmin.h"
#include <cerf.h>
#include <qmath.h>
#include <iostream>

#define SQR(x) ((x)*(x))

//! A Gaussian as a peak fit function.

class Gaussian : public PeakFunction {
public:
    void setY(const double* P, const int nXY, const double* X, double* Y) const final;
    void setDY(const double* P, const int nXY, const double* X, double* Jacobian) const final;
    int nPar() const final { return 3; }
private:
    const double prefac = 1 / sqrt(2*M_PI);
};

//! A Lorentzian as a peak fit function.

class Lorentzian : public PeakFunction {
public:
    void setY(const double* P, const int nXY, const double* X, double* Y) const final;
    void setDY(const double* P, const int nXY, const double* X, double* Jacobian) const final;
    int nPar() const final { return 3; }
};

//! A Lorentzian as a peak fit function.

class Voigt : public PeakFunction {
public:
    void setY(const double* P, const int nXY, const double* X, double* Y) const final;
    void setDY(const double* P, const int nXY, const double* X, double* Jacobian) const final;
    int nPar() const final { return 4; }
    PeakOutcome outcome(const Fitted&) const final;
};

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
//! @class PeakFunction

PeakOutcome PeakFunction::outcome(const Fitted& F) const
{
    return {
        {F.parValAt(0), F.parErrAt(0)},
        {F.parValAt(1), F.parErrAt(1)},
        {F.parValAt(2), F.parErrAt(2)} };
}

Fitted PeakFunction::fromFit(const QString& name, const Curve& curve, const RawOutcome& rawOutcome)
{
    const PeakFunction* f;
    bool onlyPositiveParams = false;
    if        (name=="Raw") {
        return {};
    } else if (name=="Gaussian") {
        f = new Gaussian;
    } else if (name=="Lorentzian") {
        f = new Lorentzian;
    } else if (name=="Voigt") {
        f = new Voigt;
        onlyPositiveParams = true;
    } else
        qFatal("Impossible case");
    std::vector<double> startParams(f->nPar(), 1.);
    startParams[0] = rawOutcome.getCenter();
    startParams[1] = rawOutcome.getFwhm();
    startParams[2] = rawOutcome.getIntensity();
    return FitWrapper().execFit(f, curve, startParams, onlyPositiveParams);
}

//  ***********************************************************************************************
//! @class Gaussian

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

PeakOutcome Voigt::outcome(const Fitted& F) const
{
    double fwhm = FindFwhm::fromFitted(F).value();
    return {
        {F.parValAt(0), F.parErrAt(0)},
        DoubleWithError{fwhm, fwhm / F.parValAt(1) * F.parErrAt(1)},
        {F.parValAt(2), F.parErrAt(2)},
        std::unique_ptr<DoubleWithError>(new DoubleWithError{F.parValAt(3), F.parErrAt(3)}) };
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
