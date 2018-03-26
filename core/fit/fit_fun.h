// ************************************************************************** //
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
// ************************************************************************** //

#ifndef FIT_FUN_H
#define FIT_FUN_H

#include "core/fit/parametric_function.h"
#include "core/typ/singleton.h"
#include "core/typ/registry.h"
#include "core/typ/curve.h"
#include "core/typ/realpair.h"
#include "core/typ/types.h"

//! A polynomial, for fitting the background of a diffractogram

class Polynom : public Function {
public:
    Polynom(int degree = 0) { setDegree(degree); }

    int degree() const;
    void setDegree(int);

    double y(double x, double const* parValues = nullptr) const;
    double dy(double x, int parIndex, double const* parValues = nullptr) const;

    double avgY(const Range&, double const* parValues = nullptr) const;

    void fit(const Curve&, const Ranges&);
    static Polynom fromFit(int degree, const Curve&, const Ranges&);

    JsonObj toJson() const final;
    void fromJson(const JsonObj&) THROWS final;

    QString name() const final { return "polynom"; }
};


//! Abstract peak function

class PeakFunction : public Function {
public:
    PeakFunction();

    void reset();
    void fit(const Curve& curve) { return fit(curve, range_); }
    virtual void fit(const Curve&, const Range&);
    void fromJson(const JsonObj&) THROWS;
    virtual void setRange(const Range& range) { range_ = range; }
    virtual void setGuessedPeak(const qpair& peak) { guessedPeak_ = peak; }
    virtual void setGuessedFWHM(const float fwhm) { guessedFWHM_ = fwhm; }

    PeakFunction* clone() const;
    const Range& range() const { return range_; }
    const qpair& guessedPeak() const { return guessedPeak_; }
    float guessedFWHM() const { return guessedFWHM_; }
    virtual qpair fittedPeak() const = 0;
    virtual float fittedFWHM() const = 0;
    virtual qpair peakError() const = 0;
    virtual float fwhmError() const = 0;
    JsonObj toJson() const final;
    virtual bool isRaw() const { return false; } //!< overwritten in class Raw, obviously

protected:
    Range range_;
    qpair guessedPeak_;
    float guessedFWHM_;

    Curve prepareFit(const Curve&, const Range&);
};


typedef class PeakFunction* (*initializer_type)();

//! Holds initializers for all available peak functions.

class FunctionRegistry : public IRegistry<initializer_type>, public ISingleton<FunctionRegistry> {
public:
    void register_fct(const initializer_type f);
    static PeakFunction* name2new(const QString&) THROWS;
    static PeakFunction* clone(const PeakFunction& old);
};

#endif // FIT_FUN_H
