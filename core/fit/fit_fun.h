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

#include "typ/singleton.h"
#include "typ/registry.h"
#include "parametric_function.h"
#include "typ/realpair.h"
#include "typ/types.h"

class Curve;

//! A polynomial, for fitting the background of a diffractogram

class Polynom final : public Function {
public:
    Polynom(uint degree = 0) { setDegree(degree); }

    uint degree() const;
    void setDegree(uint);

    qreal y(qreal x, qreal const* parValues = nullptr) const;
    qreal dy(qreal x, uint parIndex, qreal const* parValues = nullptr) const;

    qreal avgY(Range const&, qreal const* parValues = nullptr) const;

    void fit(Curve const&, Ranges const&);
    static Polynom fromFit(uint degree, Curve const&, Ranges const&);

    JsonObj to_json() const;
    void from_json(JsonObj const&) THROWS;

    str name() const { return "polynom"; }
};


//! Abstract peak function

class PeakFunction : public Function {
public:
    PeakFunction();
    PeakFunction* clone() const;

    Range const& range() const { return range_; }
    virtual void setRange(Range const&);

    virtual void setGuessedPeak(qpair const&);
    virtual void setGuessedFWHM(fwhm_t);

    qpair const& guessedPeak() const { return guessedPeak_; }
    fwhm_t guessedFWHM() const { return guessedFWHM_; }

    virtual qpair fittedPeak() const = 0;
    virtual fwhm_t fittedFWHM() const = 0;

    virtual qpair peakError() const = 0;
    virtual fwhm_t fwhmError() const = 0;

    void reset();

    void fit(Curve const& curve) { return fit(curve, range_); }

    virtual void fit(Curve const&, Range const&);

    JsonObj to_json() const final;
    void from_json(JsonObj const&) THROWS;

protected:
    Range range_;
    qpair guessedPeak_;
    fwhm_t guessedFWHM_;

    Curve prepareFit(Curve const&, Range const&);
};


typedef class PeakFunction* (*initializer_type)();

class FunctionRegistry : public IRegistry<initializer_type>, public ISingleton<FunctionRegistry> {
public:
    void register_fct(const initializer_type f);
    static PeakFunction* name2new(QString const&) THROWS;
    static PeakFunction* clone(PeakFunction const& old);
};

#endif // FIT_FUN_H
