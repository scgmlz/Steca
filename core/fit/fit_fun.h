//  ***********************************************************************************************
//
//  Steca: stress and texture calculator
//
//! @file      core/fit/fit_fun.h
//! @brief     Defines classes Polynom and PeakFunction with subclasses, and FunctionRegistry
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
//  ***********************************************************************************************

#ifndef FIT_FUN_H
#define FIT_FUN_H

#include "core/fit/parametric_function.h"
#include "core/typ/curve.h"
#include "core/typ/realpair.h"
#include "core/fit/registry.h"
#include "core/fit/singleton.h"

//! A polynomial, for fitting the background of a diffractogram

// TODO use Legendre polynomials on rescaled interval to provide an easy approximation of the average.
class Polynom : public ParametricFunction {
public:
    Polynom(int _degree) { setParameterCount(_degree + 1); }

    double y(double x, double const* parValues = nullptr) const final;

    static Polynom fromFit(int degree, const Curve&, const Ranges&);

private:
    double dy(double x, int parIndex, double const* parValues = nullptr) const final;
};


//! Abstract peak function

class PeakFunction : public ParametricFunction {
public:
    PeakFunction();

    Range& fitRange() { return range_; }

    virtual void doFit(const Curve&, const Range&);
    virtual void setRange(const Range& range) { range_ = range; }
    virtual void setGuessedPeak(const qpair& peak) { guessedPeak_ = peak; }
    virtual void setGuessedFWHM(const float fwhm) { guessedFWHM_ = fwhm; }

    PeakFunction* clone() const;
    const qpair& guessedPeak() const { return guessedPeak_; }
    float guessedFWHM() const { return guessedFWHM_; }
    virtual qpair fittedPeak() const = 0;
    virtual float fittedFWHM() const = 0;
    virtual qpair peakError() const = 0;
    virtual float fwhmError() const = 0;
    virtual bool isRaw() const { return false; } //!< overwritten in class Raw, obviously
    virtual QString name() const = 0;

protected:
    Curve prepareFit(const Curve&, const Range&);

    Range range_;
    qpair guessedPeak_;
    float guessedFWHM_;
};


typedef class PeakFunction* (*initializer_type)();

//! Holds initializers for all available peak functions.

class FunctionRegistry : public IRegistry<initializer_type>, public ISingleton<FunctionRegistry> {
public:
    void register_fct(const initializer_type f);
    static PeakFunction* name2new(const QString&);
    static PeakFunction* clone(const PeakFunction& old);
};

#endif // FIT_FUN_H
