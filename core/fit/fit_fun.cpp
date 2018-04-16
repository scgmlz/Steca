//  ***********************************************************************************************
//
//  Steca: stress and texture calculator
//
//! @file      core/fit/fit_fun.cpp
//! @brief     Implements classes Polynom and PeakFunction, and FunctionRegistry
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
//  ***********************************************************************************************

#include "fit_fun.h"
#include "qcr/engine/debug.h"
#include "core/def/idiomatic_for.h"
#include "core/fit/fit_methods.h"

namespace {

//! Compute a low power with an exponent of type int
static double pow_n(double x, int n)
{
    double val = 1;
    while (n-- > 0)
        val *= x;
    return val;
}

} // namespace

//  ***********************************************************************************************
//! @class Polynom

double Polynom::y(double x, double const* parValues) const
{
    double val = 0, xPow = 1;
    for_i (parameters_.count()) {
        val += parValue(i, parValues) * xPow;
        xPow *= x;
    }
    return val;
}

double Polynom::dy(double x, int i, double const*) const
{
    return pow_n(x, i);
}

// REVIEW
double Polynom::avgY(const Range& rgeX, double const* parValues) const
{
    ASSERT(rgeX.isValid());

    double w = rgeX.width();
    if (w <= 0)
        return y(rgeX.min, parValues);

    double minY = 0, maqpair = 0, minPow = 1, maxPow = 1;
    for_i (parameters_.count()) {
        double facY = parValue(i, parValues) / (i + 1);
        minY += facY * (minPow *= rgeX.min);
        maqpair += facY * (maxPow *= rgeX.max);
    }

    return (1 / w) * (maqpair - minY);
}

void Polynom::fit(const Curve& curve, const Ranges& ranges)
{
    FitWrapper().fit(*this, curve.intersect(ranges));
}

Polynom Polynom::fromFit(int degree, const Curve& curve, const Ranges& ranges)
{
    Polynom poly(degree);
    poly.fit(curve, ranges);
    return poly;
}

JsonObj Polynom::toJson() const
{
    JsonObj ret = ParametricFunction::toJson();
    ret.insert("type", name());
    return ret;
}

//  ***********************************************************************************************
//! @class PeakFunction

PeakFunction::PeakFunction()
    : guessedPeak_(), guessedFWHM_(Q_QNAN)
{}

void PeakFunction::fit(const Curve& curve, const Range& range)
{
    const Curve c = prepareFit(curve, range);
    if (c.isEmpty())
        return;

    //  if (!guessedPeak().isValid()) {  // calculate guesses // TODO caching
    //  temporarily disabled, until it works correctly
    const int peakIndex = c.maqpairindex();
    const double peakTth = c.x(peakIndex);
    const double peakIntens = c.y(peakIndex);

    // half-maximum indices
    int hmi1 = peakIndex, hmi2 = peakIndex;
    // left
    for (int i = peakIndex; i-- > 0;) {
        hmi1 = i;
        if (c.y(i) < peakIntens / 2)
            break;
    }
    // right
    for (int i = peakIndex, iCnt = c.count(); i < iCnt; ++i) {
        hmi2 = i;
        if (c.y(i) < peakIntens / 2)
            break;
    }

    setGuessedPeak(qpair(peakTth, peakIntens));
    setGuessedFWHM(c.x(hmi2) - c.x(hmi1));

    FitWrapper().fit(*this, c);
}

Curve PeakFunction::prepareFit(const Curve& curve, const Range& range)
{
    reset();
    return curve.intersect(range);
}

JsonObj PeakFunction::toJson() const
{
    JsonObj ret = ParametricFunction::toJson();
    ret.insert("range", range_.toJson());
    ret.insert("guessed peak", guessedPeak_.toJson());
    ret.insert("guessed fwhm", double_to_json(guessedFWHM_));
    ret.insert("type", name());
    return ret;
}

void PeakFunction::fromJson(const JsonObj& obj)
{
    ParametricFunction::fromJson(obj);
    range_ = obj.loadRange("range");
    guessedPeak_.fromJson(obj.loadObj("guessed peak"));
    guessedFWHM_ = obj.loadQreal("guessed fwhm");
}

//  ***********************************************************************************************
//  FunctionRegistry

void FunctionRegistry::register_fct(const initializer_type f)
{
    PeakFunction* tmp = f(); // implicit 'new'
    register_item(tmp->name(), f);
    delete tmp;
}

PeakFunction* FunctionRegistry::name2new(const QString& peakFunctionName)
{
    initializer_type make_new = instance()->find_or_fail(peakFunctionName);
    return make_new();
}

PeakFunction* FunctionRegistry::clone(const PeakFunction& old)
{
    PeakFunction* ret = name2new(old.name());
    *ret = old;
    return ret;
}
