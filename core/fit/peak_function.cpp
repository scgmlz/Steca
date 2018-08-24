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
#include <cmath>

#define SQR(x) (x)*(x)

//! A Gaussian as a peak fit function.

class Gaussian : public PeakFunction {
public:
    void setY(const double* P, const int nXY, const double* X, double* Y) const final;
    void setDY(const double* P, const int nXY, const double* X, double* Jacobian) const final;
    int nPar() const final { return 3; };
private:
    const double prefac = 1 / sqrt(2*M_PI);
};

//! A Lorentzian as a peak fit function.

class Lorentzian : public PeakFunction {
public:
    void setY(const double* P, const int nXY, const double* X, double* Y) const final;
    void setDY(const double* P, const int nXY, const double* X, double* Jacobian) const final;
    int nPar() const final { return 3; };
};

//  ***********************************************************************************************
//! @class PeakFunction

PeakOutcome PeakFunction::outcome(const Fitted& F) const
{
    return {
        {F.parVal.at(0), F.parErr.at(0)},
        {F.parVal.at(1), F.parErr.at(1)},
        {F.parVal.at(2), F.parErr.at(2)} };
}

Fitted PeakFunction::fromFit(const QString& name, const Curve& curve, const RawOutcome& rawOutcome)
{
    const PeakFunction* f;
    int nPar = 3;
    if        (name=="Raw") {
        return {};
    } else if (name=="Gaussian") {
        f = new Gaussian();
    } else if (name=="Lorentzian") {
        f = new Lorentzian();
    } else
        qFatal("Impossible case");
    std::vector<double> startParams(nPar);
    startParams[0] = rawOutcome.getCenter();
    startParams[1] = rawOutcome.getFwhm();
    startParams[2] = rawOutcome.getIntensity();
    return FitWrapper().execFit(f, curve, startParams);
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
