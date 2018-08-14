//  ***********************************************************************************************
//
//  Steca: stress and texture calculator
//
//! @file      core/fit/fit_fun.cpp
//! @brief     Implements classes ParametricFunction, Polynom, PeakFunction
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
//  ***********************************************************************************************

#include "fit_fun.h"
#include "core/typ/curve.h"
#include "core/fit/fit_methods.h"
#include "core/fit/raw_outcome.h"
#include "qcr/base/debug.h"

#include <cmath>

#define SQR(x) (x)*(x)

namespace {

//! Computes a low integer power of x.
static double pow_n(double x, int n)
{
    double ret = 1;
    while (n-- > 0)
        ret *= x;
    return ret;
}

} // namespace

//  ***********************************************************************************************
//! @class ParametricFunction

// outside the fit routine, functions y(x) are called with parValues==nullptr

double ParametricFunction::parValue(int ip, double const* parValues) const
{
    return parValues ? parValues[ip] : parameters_.at(ip).value();
}

void ParametricFunction::setY(
    const double* parValues, const int nPts, const double* xValues, double* yValues) const
{
    for (int i=0 ; i<nPts; ++i)
        yValues[i] = y(*(xValues+i), parValues);
}

void ParametricFunction::setDY(
    const double* parValues, const int nPar, const int nPts, const double* xValues,
    double* jacobian) const
{
    for (int i=0; i<nPts; ++i)
        for (int ip=0; ip<nPar; ++ip)
            *jacobian++ = dy(*(xValues+i), ip, parValues);
}

//  ***********************************************************************************************
//! @class Polynom

double Polynom::y(double x, double const* parValues) const
{
    double ret = 0, xPow = 1;
    for (int i=0; i<parameters_.size(); ++i) {
        ret += parValue(i, parValues) * xPow;
           // outside the fit routine, functions y(x) are called with parValues==nullptr;
           // therefore we need 'parValue' to access either 'parValues' or 'parameters_'.
        xPow *= x;
    }
    return ret;
}

double Polynom::dy(double x, int i, double const*) const
{
    return pow_n(x, i);
}

Polynom Polynom::fromFit(int degree, const Curve& curve, const Ranges& ranges)
{
    //qDebug() << "polynomial fit";
    Polynom p(degree);
    FitWrapper().execFit(p, curve.intersect(ranges));
    return p;
}

//  ***********************************************************************************************
//! @class PeakFunction

PeakFunction::PeakFunction(const QString& functionName, const RawOutcome& rawOutcome)
{
    // Gaussian
    parameters_.resize(3);
    parameters_[0].setValue(rawOutcome.getCenter(),0);
    parameters_[1].setValue(rawOutcome.getFwhm() / sqrt(8*log(2)),0);
    parameters_[2].setValue(rawOutcome.getIntensity(),0);
}

const FitParameter& PeakFunction::getCenter() const { return parameters_[0]; }
const FitParameter PeakFunction::getFwhm() const { return
        FitParameter(parameters_[1].value()*sqrt(8*log(2)),parameters_[1].error()*sqrt(8*log(2))); }
const FitParameter& PeakFunction::getIntensity() const { return parameters_[2]; }

const double prefac = 1 / sqrt(2*M_PI);

void PeakFunction::setY(
    const double* parValues, const int nPts, const double* xValues, double* yValues) const
{
    double center = parValue(0, parValues);
    double stdv = parValue(1, parValues);
    double inten = parValue(2, parValues);
    for (int i=0 ; i<nPts; ++i)
        yValues[i] = inten*prefac/stdv*exp(-SQR(*(xValues+i)-center)/(2*SQR(stdv)));
}

void PeakFunction::setDY(
    const double* parValues, const int nPar, const int nPts, const double* xValues,
    double* jacobian) const
{
    double center = parValue(0, parValues);
    double stdv = parValue(1, parValues);
    double inten = parValue(2, parValues);
    for (int i=0; i<nPts; ++i) {
        double x = *(xValues+i);
        double g = exp(-SQR(x-center)/(2*SQR(stdv)));
        *jacobian++ = inten*prefac/stdv*g*(x-center)/SQR(stdv);
        *jacobian++ = inten*prefac/stdv*g*(SQR((x-center)/stdv)-1)/stdv;
        *jacobian++ = prefac/stdv*g;
    }
}

double PeakFunction::y(double x, double const* parValues) const
{
    // Gaussian
    double center = parValue(0, parValues);
    double stdv = parValue(1, parValues);
    double inten = parValue(2, parValues);
    return inten*prefac/stdv*exp(-SQR(x-center)/(2*SQR(stdv)));
}

double PeakFunction::dy(double x, int i, double const* parValues) const
{
    qFatal("obsoleted by setDY");
}

PeakFunction PeakFunction::fromFit(
    const QString& functionName, const Curve& curve, const RawOutcome& rawOutcome)
{
    //qDebug() << "peak fit";
    ASSERT(curve.count());
    PeakFunction p(functionName, rawOutcome);
    FitWrapper().execFit(p, curve);
    return p;
}
