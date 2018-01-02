// ************************************************************************** //
//
//  Steca2: stress and texture calculator
//
//! @file      core/fit/fit_fun.h
//! @brief     Defines classes Polynom and PeakFunction with subclasses, and FunctionRegistry
//!
//! @homepage  https://github.com/scgmlz/Steca2
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2017
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
// ************************************************************************** //

#ifndef FIT_FUN_H
#define FIT_FUN_H

#include "typ/singleton.h"
#include "typ/registry.h"
#include "typ/curve.h"
#include "typ/fun.h"
#include "typ/realpair.h"
#include "typ/types.h"

//! A polynomial, for fitting the background of a diffractogram

class Polynom final : public typ::SimpleFunction {
private:
    using super = typ::SimpleFunction;
public:
    Polynom(uint degree = 0) { setDegree(degree); }

    uint degree() const;
    void setDegree(uint);

    qreal y(qreal x, qreal const* parValues = nullptr) const;
    qreal dy(qreal x, uint parIndex, qreal const* parValues = nullptr) const;

    qreal avgY(typ::Range const&, qreal const* parValues = nullptr) const;

    void fit(typ::Curve const&, typ::Ranges const&);
    static Polynom fromFit(uint degree, typ::Curve const&, typ::Ranges const&);

    typ::JsonObj to_json() const;
    void from_json(typ::JsonObj const&) THROWS;

    str name() const { return "polynom"; }
};


//! Abstract peak function

class PeakFunction : public typ::SimpleFunction {
private:
    using super = typ::SimpleFunction;
public:
    PeakFunction();
    PeakFunction* clone() const;

    typ::Range const& range() const { return range_; }
    virtual void setRange(typ::Range const&);

    virtual void setGuessedPeak(qpair const&);
    virtual void setGuessedFWHM(fwhm_t);

    qpair const& guessedPeak() const { return guessedPeak_; }
    fwhm_t guessedFWHM() const { return guessedFWHM_; }

    virtual qpair fittedPeak() const = 0;
    virtual fwhm_t fittedFWHM() const = 0;

    virtual qpair peakError() const = 0;
    virtual fwhm_t fwhmError() const = 0;

    void reset();

    void fit(typ::Curve const& curve) { return fit(curve, range_); }

    virtual void fit(typ::Curve const&, typ::Range const&);

    typ::JsonObj to_json() const final;
    void from_json(typ::JsonObj const&) THROWS;

protected:
    typ::Range range_;
    qpair guessedPeak_;
    fwhm_t guessedFWHM_;

    typ::Curve prepareFit(typ::Curve const&, typ::Range const&);
};


typedef class PeakFunction* (*initializer_type)();

namespace typ {
}

class FunctionRegistry : public IRegistry<initializer_type>, public ISingleton<FunctionRegistry> {
public:
    void register_fct(const initializer_type f);
    static PeakFunction* name2new(QString const&) THROWS;
    static PeakFunction* clone(PeakFunction const& old);
};

#endif // FIT_FUN_H
