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

    qreal y(qreal x, qreal const* parValues = nullptr) const;
    qreal dy(qreal x, int parIndex, qreal const* parValues = nullptr) const;

    qreal avgY(const Range&, qreal const* parValues = nullptr) const;

    void fit(Curve const&, const Ranges&);
    static Polynom fromFit(int degree, Curve const&, const Ranges&);

    JsonObj to_json() const final;
    void from_json(JsonObj const&) THROWS final;

    QString name() const final { return "polynom"; }
};


//! Abstract peak function

class PeakFunction : public Function {
public:
    PeakFunction();

    void reset();
    void fit(Curve const& curve) { return fit(curve, range_); }
    virtual void fit(Curve const&, const Range&);
    void from_json(JsonObj const&) THROWS;
    virtual void setRange(const Range& range) { range_ = range; }
    virtual void setGuessedPeak(const qpair& peak) { guessedPeak_ = peak; }
    virtual void setGuessedFWHM(const fwhm_t fwhm) { guessedFWHM_ = fwhm; }

    PeakFunction* clone() const;
    const Range& range() const { return range_; }
    qpair const& guessedPeak() const { return guessedPeak_; }
    fwhm_t guessedFWHM() const { return guessedFWHM_; }
    virtual qpair fittedPeak() const = 0;
    virtual fwhm_t fittedFWHM() const = 0;
    virtual qpair peakError() const = 0;
    virtual fwhm_t fwhmError() const = 0;
    JsonObj to_json() const final;
    virtual bool isRaw() const { return false; } //!< overwritten in class Raw, obviously

protected:
    Range range_;
    qpair guessedPeak_;
    fwhm_t guessedFWHM_;

    Curve prepareFit(Curve const&, const Range&);
};


typedef class PeakFunction* (*initializer_type)();

//! Holds initializers for all available peak functions.

class FunctionRegistry : public IRegistry<initializer_type>, public ISingleton<FunctionRegistry> {
public:
    void register_fct(const initializer_type f);
    static PeakFunction* name2new(const QString&) THROWS;
    static PeakFunction* clone(PeakFunction const& old);
};

#endif // FIT_FUN_H
