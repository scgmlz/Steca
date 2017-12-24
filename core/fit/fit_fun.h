// ************************************************************************** //
//
//  Steca2: stress and texture calculator
//
//! @file      core/fit/fit_fun.h
//! @brief     Defines classes Polynom and PeakFunction with subclasses
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

typedef class typ::SimpleFunction* (*initializer_type)();

class FunctionRegistry : public IRegistry<initializer_type>, public ISingleton<FunctionRegistry> {
public:
    void register_fct(const initializer_type f);
};


namespace fit {

//! a polynom(ial)

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


enum class ePeakType { RAW, GAUSSIAN, LORENTZIAN, PSEUDOVOIGT1, PSEUDOVOIGT2, NUM_TYPES };


//! Abstract peak function

class PeakFunction : public typ::SimpleFunction {
private:
    using super = typ::SimpleFunction;
public:
    static PeakFunction* factory(ePeakType);

    PeakFunction();
    PeakFunction* clone() const;

    virtual ePeakType type() const = 0;

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


//! Peak analysis without fitting

class Raw final : public PeakFunction {
private:
    using super = PeakFunction;
public:
    ePeakType type() const { return ePeakType::RAW; }

    qreal y(qreal x, qreal const* parValues = nullptr) const;
    qreal dy(qreal x, uint parIndex, qreal const* parValues = nullptr) const;

    qpair fittedPeak() const;
    fwhm_t fittedFWHM() const;

    qpair peakError() const;
    fwhm_t fwhmError() const;

    void setRange(typ::Range const&);
    void fit(typ::Curve const&, typ::Range const&);

    str name() const { return "Raw"; }

private:
    typ::Curve fittedCurve_; // saved from fitting
    void prepareY();

    mutable uint x_count_;
    mutable qreal dx_;
    mutable qreal sum_y_;
};


//! to fit peak with a Gaussian

class Gaussian final : public PeakFunction {
private:
    using super = PeakFunction;
public:
    enum { parAMPL, parXSHIFT, parSIGMA };

    Gaussian(qreal ampl = 1, qreal xShift = 0, qreal sigma = 1);

    ePeakType type() const { return ePeakType::GAUSSIAN; }

    qreal y(qreal x, qreal const* parValues = nullptr) const;
    qreal dy(qreal x, uint parIndex, qreal const* parValues = nullptr) const;

    void setGuessedPeak(qpair const&);
    void setGuessedFWHM(fwhm_t);

    qpair fittedPeak() const;
    fwhm_t fittedFWHM() const;

    qpair peakError() const;
    fwhm_t fwhmError() const;

    str name() const { return "Gaussian"; }
};


//! to fit peak with a Lorentzian

class Lorentzian final : public PeakFunction {
private:
    using super = PeakFunction;
public:
    enum { parAMPL, parXSHIFT, parGAMMA };

    Lorentzian(qreal ampl = 1, qreal xShift = 0, qreal gamma = 1);

    ePeakType type() const { return ePeakType::LORENTZIAN; }

    qreal y(qreal x, qreal const* parValues = nullptr) const;
    qreal dy(qreal x, uint parIndex, qreal const* parValues = nullptr) const;

    void setGuessedPeak(qpair const&);
    void setGuessedFWHM(fwhm_t);

    qpair fittedPeak() const;
    fwhm_t fittedFWHM() const;

    qpair peakError() const;
    fwhm_t fwhmError() const;

    str name() const { return "Lorentzian"; }
};


//! to fit peak with a sum of Gaussian and Lorentzian with shared width parameter

class PseudoVoigt1 final : public PeakFunction {
private:
    using super = PeakFunction;
public:
    enum { parAMPL, parXSHIFT, parSIGMAGAMMA, parETA };

    PseudoVoigt1(qreal ampl = 1, qreal xShift = 0, qreal sigmaGamma = 1, qreal eta = 0.1);

    ePeakType type() const { return ePeakType::PSEUDOVOIGT1; }

    qreal y(qreal x, qreal const* parValues = nullptr) const;
    qreal dy(qreal x, uint parIndex, qreal const* parValues = nullptr) const;

    void setGuessedPeak(qpair const&);
    void setGuessedFWHM(fwhm_t);

    qpair fittedPeak() const;
    fwhm_t fittedFWHM() const;

    qpair peakError() const;
    fwhm_t fwhmError() const;

    str name() const { return "PseudoVoigt1"; }
};


//! to fit peak with a sum of Gaussian and Lorentzian with independent width parameters

class PseudoVoigt2 final : public PeakFunction {
private:
    using super = PeakFunction;
public:
    enum { parAMPL, parXSHIFT, parSIGMA, parGAMMA, parETA };

    PseudoVoigt2(
        qreal ampl = 1, qreal xShift = 0, qreal sigma = 1, qreal gamma = 1, qreal eta = 0.1);

    ePeakType type() const { return ePeakType::PSEUDOVOIGT2; }

    qreal y(qreal x, qreal const* parValues = nullptr) const;
    qreal dy(qreal x, uint parIndex, qreal const* parValues = nullptr) const;

    void setGuessedPeak(qpair const&);
    void setGuessedFWHM(fwhm_t);

    qpair fittedPeak() const;
    fwhm_t fittedFWHM() const;

    qpair peakError() const;
    fwhm_t fwhmError() const;

    str name() const { return "PseudoVoigt2"; }}
;

} // namespace fit

#endif // FIT_FUN_H
