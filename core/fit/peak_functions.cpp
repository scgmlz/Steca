// ************************************************************************** //
//
//  Steca: stress and texture calculator
//
//! @file      core/fit/peak_functions.cpp
//! @brief     Implements specific peak function classes, and function to register them
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
// ************************************************************************** //

#include "peak_functions.h"
#include <QtMath>

namespace { // file scope

// ************************************************************************** //
//  class Raw
// ************************************************************************** //

//! Peak analysis without fitting

class Raw : public PeakFunction {
public:
    qreal y(qreal x, qreal const* parValues = nullptr) const;
    qreal dy(qreal x, int parIndex, qreal const* parValues = nullptr) const;

    qpair fittedPeak() const;
    fwhm_t fittedFWHM() const;

    qpair peakError() const;
    fwhm_t fwhmError() const;

    void setRange(const Range&);
    void fit(const Curve&, const Range&);

    QString name() const final { return "Raw"; }
    bool isRaw() const final { return true; }

private:
    Curve fittedCurve_; // saved from fitting
    void prepareY();

    mutable int x_count_;
    mutable qreal dx_;
    mutable qreal sum_y_;
};

qreal Raw::y(qreal x, qreal const* /*parValues*/) const {
    if (!x_count_ || !range_.contains(x))
        return 0;

    int i = qBound(0, qFloor((x - range_.min) / dx_), x_count_ - 1);
    return fittedCurve_.y(i);
}

qreal Raw::dy(qreal, int, qreal const*) const {
    return 0; // fake
}

qpair Raw::fittedPeak() const {
    if (qIsNaN(sum_y_) && fittedCurve_.count()) ;
    return qpair(range_.center(), sum_y_);
}

fwhm_t Raw::fittedFWHM() const {
    return fwhm_t(range_.width()); // kind-of
}

qpair Raw::peakError() const {
    return qpair(0, 0);
}

fwhm_t Raw::fwhmError() const {
    return 0;
}

void Raw::setRange(const Range& range) {
    PeakFunction::setRange(range);
    prepareY();
}

void Raw::fit(const Curve& curve, const Range& range) {
    fittedCurve_ = prepareFit(curve, range); // do no more than this
    prepareY();
}

void Raw::prepareY() {
    if (range_.isEmpty() || fittedCurve_.isEmpty()) {
        x_count_ = 0;
        dx_ = 0;
        sum_y_ = NAN;
    } else {
        x_count_ = fittedCurve_.count();
        dx_ = range_.width() / x_count_;
        sum_y_ = fittedCurve_.sumY();
    }
}


// ************************************************************************** //
//  class Gaussian
// ************************************************************************** //

//! to fit peak with a Gaussian

class Gaussian : public PeakFunction {
public:
    enum { parAMPL, parXSHIFT, parSIGMA };

    Gaussian(qreal ampl = 1, qreal xShift = 0, qreal sigma = 1);

    qreal y(qreal x, qreal const* parValues = nullptr) const;
    qreal dy(qreal x, int parIndex, qreal const* parValues = nullptr) const;

    void setGuessedPeak(const qpair&);
    void setGuessedFWHM(fwhm_t);

    qpair fittedPeak() const;
    fwhm_t fittedFWHM() const;

    qpair peakError() const;
    fwhm_t fwhmError() const;

    QString name() const final { return "Gaussian"; }
};

Gaussian::Gaussian(qreal ampl, qreal xShift, qreal sigma) {
    setParameterCount(3);

    auto& parAmpl = parameters_[parAMPL];
    auto& parXShift = parameters_[parXSHIFT];
    auto& parSigma = parameters_[parSIGMA];

    parAmpl.setValueRange(0, INF);
    parAmpl.setValue(ampl, 0);

    parXShift.setValue(xShift, 0);

    parSigma.setValueRange(0, INF);
    parSigma.setValue(sigma, 0);
}

qreal Gaussian::y(qreal x, qreal const* parValues) const {
    qreal ampl = parValue(parAMPL, parValues);
    qreal xShift = parValue(parXSHIFT, parValues);
    qreal sigma = parValue(parSIGMA, parValues);

    qreal arg = (x - xShift) / sigma;
    qreal exa = exp(-0.5 * arg * arg);

    return ampl * exa;
}

qreal Gaussian::dy(qreal x, int parIndex, qreal const* parValues) const {
    qreal ampl = parValue(parAMPL, parValues);
    qreal xShift = parValue(parXSHIFT, parValues);
    qreal sigma = parValue(parSIGMA, parValues);

    qreal arg = (x - xShift) / sigma;
    qreal exa = exp(-0.5 * arg * arg);

    switch (parIndex) {
    case parAMPL: return exa;
    case parXSHIFT: return ampl * exa * (x - xShift) / (sigma * sigma);
    case parSIGMA: return ampl * exa * ((x - xShift) * (x - xShift)) / (sigma * sigma * sigma);
    }
    qFatal("impossible case");
}

void Gaussian::setGuessedPeak(const qpair& qpair) {
    PeakFunction::setGuessedPeak(qpair);
    setValue(parXSHIFT, qpair.x);
    setValue(parAMPL, qpair.y);
}

void Gaussian::setGuessedFWHM(fwhm_t fwhm) {
    PeakFunction::setGuessedFWHM(fwhm);
    // sigma = FWHM * 1/4 * (SQRT(2)/SQRT(ln(2))) = FWHM * 0.424661
    setValue(parSIGMA, fwhm * 0.424661);
}

qpair Gaussian::fittedPeak() const {
    return qpair(parameters_.at(parXSHIFT).value(), parameters_.at(parAMPL).value());
}

fwhm_t Gaussian::fittedFWHM() const {
    return fwhm_t(parameters_.at(parSIGMA).value() / 0.424661);
}

qpair Gaussian::peakError() const {
    return qpair(parameters_.at(parXSHIFT).error(), parameters_.at(parAMPL).error());
}

fwhm_t Gaussian::fwhmError() const {
    // TODO REVIEW
    return fwhm_t(parameters_.at(parSIGMA).error());
}


// ************************************************************************** //
//  class Lorentzian
// ************************************************************************** //

//! to fit peak with a Lorentzian

class Lorentzian : public PeakFunction {
public:
    enum { parAMPL, parXSHIFT, parGAMMA };

    Lorentzian(qreal ampl = 1, qreal xShift = 0, qreal gamma = 1);

    qreal y(qreal x, qreal const* parValues = nullptr) const;
    qreal dy(qreal x, int parIndex, qreal const* parValues = nullptr) const;

    void setGuessedPeak(const qpair&);
    void setGuessedFWHM(fwhm_t);

    qpair fittedPeak() const;
    fwhm_t fittedFWHM() const;

    qpair peakError() const;
    fwhm_t fwhmError() const;

    QString name() const final { return "Lorentzian"; }
};

Lorentzian::Lorentzian(qreal ampl, qreal xShift, qreal gamma) {
    setParameterCount(3);

    auto& parAmpl = parameters_[parAMPL];
    auto& parXShift = parameters_[parXSHIFT];
    auto& parGamma = parameters_[parGAMMA];

    parAmpl.setValueRange(0, INF);
    parAmpl.setValue(ampl, 0);

    parXShift.setValue(xShift, 0);

    parGamma.setValueRange(0, INF);
    parGamma.setValue(gamma, 0);
}

qreal Lorentzian::y(qreal x, qreal const* parValues) const {
    qreal ampl = parValue(parAMPL, parValues);
    qreal xShift = parValue(parXSHIFT, parValues);
    qreal gamma = parValue(parGAMMA, parValues);

    qreal arg = (x - xShift) / gamma;
    return ampl / (1 + arg * arg);
}

qreal Lorentzian::dy(qreal x, int parIndex, qreal const* parValues) const {
    qreal ampl = parValue(parAMPL, parValues);
    qreal xShift = parValue(parXSHIFT, parValues);
    qreal gamma = parValue(parGAMMA, parValues);

    qreal arg1 = (x - xShift) / gamma;
    qreal arg2 = arg1 * arg1;
    qreal arg3 = (1 + arg2) * (1 + arg2);

    switch (parIndex) {
    case parAMPL: return 1 / (1 + arg2);
    case parXSHIFT: return 2 * ampl * (x - xShift) / (arg3 * gamma * gamma);
    case parGAMMA: return 2 * ampl * (x - xShift) * (x - xShift) / (arg3 * gamma * gamma * gamma);
    }
    qFatal("impossible case");
}

void Lorentzian::setGuessedPeak(const qpair& qpair) {
    PeakFunction::setGuessedPeak(qpair);
    setValue(parXSHIFT, qpair.x);
    setValue(parAMPL, qpair.y);
}

void Lorentzian::setGuessedFWHM(fwhm_t fwhm) {
    PeakFunction::setGuessedFWHM(fwhm);
    // gamma = HWHM = FWHM / 2
    setValue(parGAMMA, fwhm / 2);
}

qpair Lorentzian::fittedPeak() const {
    return qpair(parameters_.at(parXSHIFT).value(), parameters_.at(parAMPL).value());
}

fwhm_t Lorentzian::fittedFWHM() const {
    return fwhm_t(parameters_.at(parGAMMA).value() * 2);
}

qpair Lorentzian::peakError() const {
    return qpair(parameters_.at(parXSHIFT).error(), parameters_.at(parAMPL).error());
}

fwhm_t Lorentzian::fwhmError() const {
    return fwhm_t(parameters_.at(parGAMMA).error());
}


// ************************************************************************** //
//  class PseudoVoigt1
// ************************************************************************** //


//! to fit peak with a sum of Gaussian and Lorentzian with shared width parameter

class PseudoVoigt1 : public PeakFunction {
public:
    enum { parAMPL, parXSHIFT, parSIGMAGAMMA, parETA };

    PseudoVoigt1(qreal ampl = 1, qreal xShift = 0, qreal sigmaGamma = 1, qreal eta = 0.1);

    qreal y(qreal x, qreal const* parValues = nullptr) const;
    qreal dy(qreal x, int parIndex, qreal const* parValues = nullptr) const;

    void setGuessedPeak(const qpair&);
    void setGuessedFWHM(fwhm_t);

    qpair fittedPeak() const;
    fwhm_t fittedFWHM() const;

    qpair peakError() const;
    fwhm_t fwhmError() const;

    QString name() const final { return "PseudoVoigt1"; }
};

PseudoVoigt1::PseudoVoigt1(qreal ampl, qreal xShift, qreal sigmaGamma, qreal eta) {
    setParameterCount(4);

    auto& parAmpl = parameters_[parAMPL];
    auto& parXShift = parameters_[parXSHIFT];
    auto& parSigmaGamma = parameters_[parSIGMAGAMMA];
    auto& parEta = parameters_[parETA];

    parAmpl.setValueRange(0, INF);
    parAmpl.setValue(ampl, 0);

    parXShift.setValue(xShift, 0);

    parSigmaGamma.setValueRange(0, INF);
    parSigmaGamma.setValue(sigmaGamma, 0);

    parEta.setValueRange(0, 1);
    parEta.setValue(eta, 0);
}

qreal PseudoVoigt1::y(qreal x, qreal const* parValues) const {
    qreal ampl = parValue(parAMPL, parValues);
    qreal xShift = parValue(parXSHIFT, parValues);
    qreal sigmaGamma = parValue(parSIGMAGAMMA, parValues);
    qreal eta = parValue(parETA, parValues);

    qreal arg = (x - xShift) / sigmaGamma;
    qreal arg2 = arg * arg;
    qreal gaussian = ampl * exp(-arg2 * log(2.0));
    qreal lorentz = ampl / (1 + arg2);

    return (1 - eta) * gaussian + eta * lorentz;
}

qreal PseudoVoigt1::dy(qreal x, int parIndex, qreal const* parValues) const {
    qreal ampl = parValue(parAMPL, parValues);
    qreal xShift = parValue(parXSHIFT, parValues);
    qreal sigmaGamma = parValue(parSIGMAGAMMA, parValues);
    qreal eta = parValue(parETA, parValues);

    qreal arg1 = (x - xShift) / sigmaGamma;
    qreal arg2 = arg1 * arg1;
    qreal arg3 = exp(-arg2 * log(2.0));
    qreal arg4 = 1 + arg2;

    switch (parIndex) {
    case parAMPL: return eta / arg4 + (1 - eta) * arg3;
    case parXSHIFT:
        return eta * 2 * ampl * (x - xShift) / (arg4 * arg4 * sigmaGamma * sigmaGamma)
            + (1 - eta) * 2 * ampl * (x - xShift) * log(2.0) * arg3 / (sigmaGamma * sigmaGamma);
    case parSIGMAGAMMA:
        return eta * 2 * ampl * (x - xShift) * (x - xShift)
            / (arg4 * arg4 * sigmaGamma * sigmaGamma * sigmaGamma)
            + (1 - eta) * 2 * ampl * (x - xShift) * (x - xShift) * log(2.0) * arg3
            / (sigmaGamma * sigmaGamma * sigmaGamma);
    case parETA: return ampl / arg4 - ampl * arg3;
    }
    qFatal("impossible case");
}

void PseudoVoigt1::setGuessedPeak(const qpair& qpair) {
    PeakFunction::setGuessedPeak(qpair);
    setValue(parXSHIFT, qpair.x);
    setValue(parAMPL, qpair.y);
}

void PseudoVoigt1::setGuessedFWHM(fwhm_t fwhm) {
    PeakFunction::setGuessedFWHM(fwhm);
    setValue(parSIGMAGAMMA, fwhm / 2);
}

qpair PseudoVoigt1::fittedPeak() const {
    return qpair(parameters_.at(parXSHIFT).value(), parameters_.at(parAMPL).value());
}

fwhm_t PseudoVoigt1::fittedFWHM() const {
    return fwhm_t(parameters_.at(parSIGMAGAMMA).value() * 2);
}

qpair PseudoVoigt1::peakError() const {
    return qpair(parameters_.at(parXSHIFT).error(), parameters_.at(parAMPL).error());
}

fwhm_t PseudoVoigt1::fwhmError() const {
    return fwhm_t(parameters_.at(parSIGMAGAMMA).error());
}


// ************************************************************************** //
//  class PseudoVoigt2
// ************************************************************************** //

//! to fit peak with a sum of Gaussian and Lorentzian with independent width parameters

class PseudoVoigt2 : public PeakFunction {
public:
    enum { parAMPL, parXSHIFT, parSIGMA, parGAMMA, parETA };

    PseudoVoigt2(
        qreal ampl = 1, qreal xShift = 0, qreal sigma = 1, qreal gamma = 1, qreal eta = 0.1);

    qreal y(qreal x, qreal const* parValues = nullptr) const;
    qreal dy(qreal x, int parIndex, qreal const* parValues = nullptr) const;

    void setGuessedPeak(const qpair&);
    void setGuessedFWHM(fwhm_t);

    qpair fittedPeak() const;
    fwhm_t fittedFWHM() const;

    qpair peakError() const;
    fwhm_t fwhmError() const;

    QString name() const final { return "PseudoVoigt2"; }
};

PseudoVoigt2::PseudoVoigt2(qreal ampl, qreal mu, qreal hwhmG, qreal hwhmL, qreal eta) {
    setParameterCount(5);

    auto& parAmpl = parameters_[parAMPL];
    auto& parMu = parameters_[parXSHIFT];
    auto& parHwhmG = parameters_[parSIGMA];
    auto& parHwhmL = parameters_[parGAMMA];
    auto& parEta = parameters_[parETA];

    parAmpl.setValueRange(0, INF);
    parAmpl.setValue(ampl, 0);

    parMu.setValue(mu, 0);

    parHwhmG.setValueRange(0, INF);
    parHwhmG.setValue(hwhmG, 0);

    parHwhmL.setValueRange(0, INF);
    parHwhmL.setValue(hwhmL, 0);

    parEta.setValueRange(0, 1);
    parEta.setValue(eta, 0);
}

qreal PseudoVoigt2::y(qreal x, qreal const* parValues) const {
    qreal ampl = parValue(parAMPL, parValues);
    qreal xShift = parValue(parXSHIFT, parValues);
    qreal sigma = parValue(parSIGMA, parValues);
    qreal gamma = parValue(parGAMMA, parValues);
    qreal eta = parValue(parETA, parValues);

    qreal argG = (x - xShift) / sigma;
    qreal argG2 = argG * argG;
    qreal gaussian = ampl * exp(-argG2 * log(2.0));

    qreal argL = (x - xShift) / gamma;
    qreal argL2 = argL * argL;
    qreal lorentz = ampl / (1 + argL2);

    return (1 - eta) * gaussian + eta * lorentz;
}

qreal PseudoVoigt2::dy(qreal x, int parIndex, qreal const* parValues) const {
    qreal ampl = parValue(parAMPL, parValues);
    qreal xShift = parValue(parXSHIFT, parValues);
    qreal sigma = parValue(parSIGMA, parValues);
    qreal gamma = parValue(parGAMMA, parValues);
    qreal eta = parValue(parETA, parValues);

    qreal argG1 = (x - xShift) / sigma;
    qreal argG2 = argG1 * argG1;
    qreal argG3 = exp(-argG2 * log(2.0));

    qreal argL1 = (x - xShift) / gamma;
    qreal argL2 = argL1 * argL1;
    qreal argL3 = 1 + argL2;

    switch (parIndex) {
    case parAMPL: return eta / argL3 + (1 - eta) * argG3;
    case parXSHIFT:
        return eta * 2 * ampl * (x - xShift) / (argL3 * argL3 * gamma * gamma)
            + (1 - eta) * 2 * ampl * (x - xShift) * log(2.0) * argG3 / (sigma * sigma);
    case parSIGMA:
        return (1 - eta) * 2 * ampl * (x - xShift) * (x - xShift) * log(2.0) * argG3
            / (sigma * sigma * sigma);
    case parGAMMA:
        return eta * 2 * ampl * (x - xShift) * (x - xShift)
            / (argL3 * argL3 * gamma * gamma * gamma);
    case parETA: return ampl / argL3 - ampl * argG3;
    }
    qFatal("impossible case");
}

void PseudoVoigt2::setGuessedPeak(const qpair& qpair) {
    PeakFunction::setGuessedPeak(qpair);
    setValue(parXSHIFT, qpair.x);
    setValue(parAMPL, qpair.y);
}

void PseudoVoigt2::setGuessedFWHM(fwhm_t fwhm) {
    PeakFunction::setGuessedFWHM(fwhm);
    setValue(parSIGMA, fwhm * 0.424661);
    setValue(parGAMMA, fwhm / 2);
}

qpair PseudoVoigt2::fittedPeak() const {
    return qpair(parameters_.at(parXSHIFT).value(), parameters_.at(parAMPL).value());
}

fwhm_t PseudoVoigt2::fittedFWHM() const {
    qreal eta = parameters_.at(parETA).value();
    return fwhm_t(
        ((1 - eta) * parameters_.at(parSIGMA).value() / 0.424661
         + eta * parameters_.at(parGAMMA).value() * 2)
        / 2);
}

qpair PseudoVoigt2::peakError() const {
    return qpair(parameters_.at(parXSHIFT).error(), parameters_.at(parAMPL).error());
}

fwhm_t PseudoVoigt2::fwhmError() const {
    // REVIEW
    return fwhm_t(parameters_.at(parSIGMA).error() + parameters_.at(parGAMMA).error());
}

} // local methods

// ************************************************************************** //
//!  Register peak functions
// ************************************************************************** //

void register_peak_functions() {
    auto G = FunctionRegistry::instance();
    G->register_fct([]()->PeakFunction*{return new Raw();});
    G->register_fct([]()->PeakFunction*{return new Gaussian();});
    G->register_fct([]()->PeakFunction*{return new Lorentzian();});
    G->register_fct([]()->PeakFunction*{return new PseudoVoigt1();});
    G->register_fct([]()->PeakFunction*{return new PseudoVoigt2();});
}
