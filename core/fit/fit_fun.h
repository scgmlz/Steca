// ************************************************************************** //
//
//  Steca2: stress and texture calculator
//
//! @file      core/fit/fit_fun.h
//! @brief     Defines ...
//!
//! @homepage  https://github.com/scgmlz/Steca2
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2017
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
// ************************************************************************** //

#ifndef FIT_FUN_H
#define FIT_FUN_H

#include "typ/typ_curve.h"
#include "typ/typ_fun.h"
#include "typ/typ_types.h"

namespace json_fun_key {
extern str const POLYNOM, RAW, GAUSSIAN, LORENTZIAN, PSEUDOVOIGT1, PSEUDOVOIGT2;
}

namespace fit {

void initFactory();

// a polynom(ial)

class Polynom : public typ::SimpleFunction {
    CLASS(Polynom) SUPER(typ::SimpleFunction) public : Polynom(uint degree = 0);

    uint degree() const;
    void setDegree(uint);

    qreal y(qreal x, qreal const* parValues = nullptr) const;
    qreal dy(qreal x, uint parIndex, qreal const* parValues = nullptr) const;

    qreal avgY(typ::Range::rc, qreal const* parValues = nullptr) const;

    void fit(typ::Curve::rc, typ::Ranges::rc);
    static Polynom fromFit(uint degree, typ::Curve::rc, typ::Ranges::rc);

public:
    typ::JsonObj saveJson() const;
    void loadJson(typ::JsonObj::rc) THROWS;
};

// Abstract peak function

enum class ePeakType { RAW, GAUSSIAN, LORENTZIAN, PSEUDOVOIGT1, PSEUDOVOIGT2, NUM_TYPES };

class PeakFunction : public typ::SimpleFunction {
    CLASS(PeakFunction)
    SUPER(typ::SimpleFunction)
    public :

        static PeakFunction* factory(ePeakType);

    PeakFunction();
    PeakFunction* clone() const;

    virtual ePeakType type() const = 0;

    typ::Range::rc range() const { return range_; }
    virtual void setRange(typ::Range::rc);

    virtual void setGuessedPeak(peak_t::rc);
    virtual void setGuessedFWHM(fwhm_t);

    peak_t::rc guessedPeak() const { return guessedPeak_; }
    fwhm_t guessedFWHM() const { return guessedFWHM_; }

    virtual peak_t fittedPeak() const = 0;
    virtual fwhm_t fittedFWHM() const = 0;

    virtual peak_t peakError() const = 0;
    virtual fwhm_t fwhmError() const = 0;

    void reset();

    void fit(typ::Curve::rc curve) { return fit(curve, range_); }

    virtual void fit(typ::Curve::rc, typ::Range::rc);

protected:
    typ::Curve prepareFit(typ::Curve::rc, typ::Range::rc);

public:
    typ::JsonObj saveJson() const;
    void loadJson(typ::JsonObj::rc) THROWS;

protected:
    typ::Range range_;
    peak_t guessedPeak_;
    fwhm_t guessedFWHM_;
};

class Raw : public PeakFunction {
    CLASS(Raw) SUPER(PeakFunction) public : Raw();

    ePeakType type() const { return ePeakType::RAW; }

    qreal y(qreal x, qreal const* parValues = nullptr) const;
    qreal dy(qreal x, uint parIndex, qreal const* parValues = nullptr) const;

    peak_t fittedPeak() const;
    fwhm_t fittedFWHM() const;

    peak_t peakError() const;
    fwhm_t fwhmError() const;

    void setRange(typ::Range::rc);
    void fit(typ::Curve::rc, typ::Range::rc);

private:
    typ::Curve fittedCurve_; // saved from fitting
    void prepareY();

    mutable uint x_count_;
    mutable qreal dx_;
    mutable qreal sum_y_;

public:
    typ::JsonObj saveJson() const;
};

class Gaussian : public PeakFunction {
    CLASS(Gaussian)
    SUPER(PeakFunction) public : enum { parAMPL, parXSHIFT, parSIGMA };

    Gaussian(qreal ampl = 1, qreal xShift = 0, qreal sigma = 1);

    ePeakType type() const { return ePeakType::GAUSSIAN; }

    qreal y(qreal x, qreal const* parValues = nullptr) const;
    qreal dy(qreal x, uint parIndex, qreal const* parValues = nullptr) const;

    void setGuessedPeak(peak_t::rc);
    void setGuessedFWHM(fwhm_t);

    peak_t fittedPeak() const;
    fwhm_t fittedFWHM() const;

    peak_t peakError() const;
    fwhm_t fwhmError() const;

public:
    typ::JsonObj saveJson() const;
};

class Lorentzian : public PeakFunction {
    CLASS(Lorentzian)
    SUPER(PeakFunction) public : enum { parAMPL, parXSHIFT, parGAMMA };

    Lorentzian(qreal ampl = 1, qreal xShift = 0, qreal gamma = 1);

    ePeakType type() const { return ePeakType::LORENTZIAN; }

    qreal y(qreal x, qreal const* parValues = nullptr) const;
    qreal dy(qreal x, uint parIndex, qreal const* parValues = nullptr) const;

    void setGuessedPeak(peak_t::rc);
    void setGuessedFWHM(fwhm_t);

    peak_t fittedPeak() const;
    fwhm_t fittedFWHM() const;

    peak_t peakError() const;
    fwhm_t fwhmError() const;

public:
    typ::JsonObj saveJson() const;
};

class PseudoVoigt1 : public PeakFunction {
    CLASS(PseudoVoigt1)
    SUPER(PeakFunction) public : enum { parAMPL, parXSHIFT, parSIGMAGAMMA, parETA };

    PseudoVoigt1(qreal ampl = 1, qreal xShift = 0, qreal sigmaGamma = 1, qreal eta = 0.1);

    ePeakType type() const { return ePeakType::PSEUDOVOIGT1; }

    qreal y(qreal x, qreal const* parValues = nullptr) const;
    qreal dy(qreal x, uint parIndex, qreal const* parValues = nullptr) const;

    void setGuessedPeak(peak_t::rc);
    void setGuessedFWHM(fwhm_t);

    peak_t fittedPeak() const;
    fwhm_t fittedFWHM() const;

    peak_t peakError() const;
    fwhm_t fwhmError() const;

public:
    typ::JsonObj saveJson() const;
};

class PseudoVoigt2 : public PeakFunction {
    CLASS(PseudoVoigt2)
    SUPER(PeakFunction) public : enum { parAMPL, parXSHIFT, parSIGMA, parGAMMA, parETA };

    PseudoVoigt2(
        qreal ampl = 1, qreal xShift = 0, qreal sigma = 1, qreal gamma = 1, qreal eta = 0.1);

    ePeakType type() const { return ePeakType::PSEUDOVOIGT2; }

    qreal y(qreal x, qreal const* parValues = nullptr) const;
    qreal dy(qreal x, uint parIndex, qreal const* parValues = nullptr) const;

    void setGuessedPeak(peak_t::rc);
    void setGuessedFWHM(fwhm_t);

    peak_t fittedPeak() const;
    fwhm_t fittedFWHM() const;

    peak_t peakError() const;
    fwhm_t fwhmError() const;

public:
    typ::JsonObj saveJson() const;
};

} // namespace fit

#endif // FIT_FUN_H
