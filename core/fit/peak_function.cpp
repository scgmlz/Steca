//  ***********************************************************************************************
//
//  Steca: stress and texture calculator
//
//! @file      core/fit/peak_function.cpp
//! @brief     Implements class PeakFunction; defines and implements its subclasses
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
//  ***********************************************************************************************

#include "core/fit/peak_function.h"
#include "core/typ/curve.h"
#include "core/fit/fit_methods.h"
#include "core/fit/raw_outcome.h"
#include "qcr/base/debug.h" // ASSERT
#include <cmath>

#define SQR(x) (x)*(x)

//! A Gaussian as a peak fit function.

class Gaussian : public PeakFunction {
public:
    void setY(const double* P, const int nXY, const double* X, double* Y) const final;
    void setDY(const double* P, const int nXY, const double* X, double* Jacobian) const final;
    int nPar() const final { return 3; };
    PeakOutcome outcome(const Fitted&) const final;
};

//  ***********************************************************************************************
//! @class Gaussian

const double prefac = 1 / sqrt(2*M_PI);

void Gaussian::setY(const double* P, const int nXY, const double* X, double* Y) const
{
    double center = P[0];
    double stdv   = P[1];
    double inten  = P[2];
    for (int i=0 ; i<nXY; ++i)
        Y[i] = inten*prefac/stdv*exp(-SQR(*(X+i)-center)/(2*SQR(stdv)));
}

void Gaussian::setDY(const double* P, const int nXY, const double* X, double* Jacobian) const
{
    double center = P[0];
    double stdv   = P[1];
    double inten  = P[2];
    for (int i=0; i<nXY; ++i) {
        double x = *(X+i);
        double g = prefac/stdv*exp(-SQR(x-center)/(2*SQR(stdv)));
        *Jacobian++ = inten*g*(x-center)/SQR(stdv);
        *Jacobian++ = inten*g*(SQR((x-center)/stdv)-1)/stdv;
        *Jacobian++ = g;
    }
}

PeakOutcome Gaussian::outcome(const Fitted& F) const
{
    return {
        {F.parVal.at(0), F.parErr.at(0)},
        {F.parVal.at(1)*sqrt(8*log(2)), F.parErr.at(2)*sqrt(8*log(2))},
        {F.parVal.at(2), F.parErr.at(2)} };
}

/*

namespace {

//  ***********************************************************************************************
//! @class Lorentzian

//! to fit peak with a Lorentzian

class Lorentzian : public PeakFunction {
public:
    enum { parAMPL, parXSHIFT, parGAMMA };

    Lorentzian(double ampl = 1, double xShift = 0, double gamma = 1);

    double y(double x, double const* P = nullptr) const final;
    double dy(double x, int parIndex, double const* P = nullptr) const final;

    void setGuessedPeak(const qpair&);
    void setGuessedFWHM(float);

    qpair fittedPeak() const;
    float fittedFWHM() const;

    qpair peakError() const;
    float fwhmError() const;

    QString name() const final { return "Lorentzian"; }
};

Lorentzian::Lorentzian(double ampl, double xShift, double gamma)
{
    setParameterCount(3);

    auto& parAmpl = parameters_[parAMPL];
    auto& parXShift = parameters_[parXSHIFT];
    auto& parGamma = parameters_[parGAMMA];

    parAmpl.setAllowedRange(0, Q_INFINITY);
    parAmpl.setValue(ampl, 0);

    parXShift.setValue(xShift, 0);

    parGamma.setAllowedRange(0, Q_INFINITY);
    parGamma.setValue(gamma, 0);
}

double Lorentzian::y(double x, double const* P) const {
    double ampl = parValue(parAMPL, P);
    double xShift = parValue(parXSHIFT, P);
    double gamma = parValue(parGAMMA, P);

    double arg = (x - xShift) / gamma;
    return ampl / (1 + arg * arg);
}

double Lorentzian::dy(double x, int parIndex, double const* P) const
{
    double ampl = parValue(parAMPL, P);
    double xShift = parValue(parXSHIFT, P);
    double gamma = parValue(parGAMMA, P);

    double arg1 = (x - xShift) / gamma;
    double arg2 = arg1 * arg1;
    double arg3 = (1 + arg2) * (1 + arg2);

    switch (parIndex) {
    case parAMPL: return 1 / (1 + arg2);
    case parXSHIFT: return 2 * ampl * (x - xShift) / (arg3 * gamma * gamma);
    case parGAMMA: return 2 * ampl * (x - xShift) * (x - xShift) / (arg3 * gamma * gamma * gamma);
    }
    qFatal("impossible case");
}

void Lorentzian::setGuessedPeak(const qpair& qpair)
{
    PeakFunction::setGuessedPeak(qpair);
    setParValue(parXSHIFT, qpair.x);
    setParValue(parAMPL, qpair.y);
}

void Lorentzian::setGuessedFWHM(float fwhm)
{
    PeakFunction::setGuessedFWHM(fwhm);
    // gamma = HWHM = FWHM / 2
    setParValue(parGAMMA, fwhm / 2);
}

qpair Lorentzian::fittedPeak() const
{
    return qpair(parameters_.at(parXSHIFT).value(), parameters_.at(parAMPL).value());
}

float Lorentzian::fittedFWHM() const
{
    return float(parameters_.at(parGAMMA).value() * 2);
}

qpair Lorentzian::peakError() const
{
    return qpair(parameters_.at(parXSHIFT).error(), parameters_.at(parAMPL).error());
}

float Lorentzian::fwhmError() const
{
    return float(parameters_.at(parGAMMA).error());
}


//  ***********************************************************************************************
//! @class PseudoVoigt1


//! to fit peak with a sum of Gaussian and Lorentzian with shared width parameter

class PseudoVoigt1 : public PeakFunction {
public:
    enum { parAMPL, parXSHIFT, parSIGMAGAMMA, parETA };

    PseudoVoigt1(double ampl = 1, double xShift = 0, double sigmaGamma = 1, double eta = 0.1);

    double y(double x, double const* P = nullptr) const final;
    double dy(double x, int parIndex, double const* P = nullptr) const final;

    void setGuessedPeak(const qpair&);
    void setGuessedFWHM(float);

    qpair fittedPeak() const;
    float fittedFWHM() const;

    qpair peakError() const;
    float fwhmError() const;

    QString name() const final { return "PseudoVoigt1"; }
};

PseudoVoigt1::PseudoVoigt1(double ampl, double xShift, double sigmaGamma, double eta)
{
    setParameterCount(4);

    auto& parAmpl = parameters_[parAMPL];
    auto& parXShift = parameters_[parXSHIFT];
    auto& parSigmaGamma = parameters_[parSIGMAGAMMA];
    auto& parEta = parameters_[parETA];

    parAmpl.setAllowedRange(0, Q_INFINITY);
    parAmpl.setValue(ampl, 0);

    parXShift.setValue(xShift, 0);

    parSigmaGamma.setAllowedRange(0, Q_INFINITY);
    parSigmaGamma.setValue(sigmaGamma, 0);

    parEta.setAllowedRange(0, 1);
    parEta.setValue(eta, 0);
}

double PseudoVoigt1::y(double x, double const* P) const
{
    double ampl = parValue(parAMPL, P);
    double xShift = parValue(parXSHIFT, P);
    double sigmaGamma = parValue(parSIGMAGAMMA, P);
    double eta = parValue(parETA, P);

    double arg = (x - xShift) / sigmaGamma;
    double arg2 = arg * arg;
    double gaussian = ampl * exp(-arg2 * log(2.0));
    double lorentz = ampl / (1 + arg2);

    return (1 - eta) * gaussian + eta * lorentz;
}

double PseudoVoigt1::dy(double x, int parIndex, double const* P) const
{
    double ampl = parValue(parAMPL, P);
    double xShift = parValue(parXSHIFT, P);
    double sigmaGamma = parValue(parSIGMAGAMMA, P);
    double eta = parValue(parETA, P);

    double arg1 = (x - xShift) / sigmaGamma;
    double arg2 = arg1 * arg1;
    double arg3 = exp(-arg2 * log(2.0));
    double arg4 = 1 + arg2;

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

void PseudoVoigt1::setGuessedPeak(const qpair& qpair)
{
    PeakFunction::setGuessedPeak(qpair);
    setParValue(parXSHIFT, qpair.x);
    setParValue(parAMPL, qpair.y);
}

void PseudoVoigt1::setGuessedFWHM(float fwhm)
{
    PeakFunction::setGuessedFWHM(fwhm);
    setParValue(parSIGMAGAMMA, fwhm / 2);
}

qpair PseudoVoigt1::fittedPeak() const
{
    return qpair(parameters_.at(parXSHIFT).value(), parameters_.at(parAMPL).value());
}

float PseudoVoigt1::fittedFWHM() const
{
    return float(parameters_.at(parSIGMAGAMMA).value() * 2);
}

qpair PseudoVoigt1::peakError() const
{
    return qpair(parameters_.at(parXSHIFT).error(), parameters_.at(parAMPL).error());
}

float PseudoVoigt1::fwhmError() const
{
    return float(parameters_.at(parSIGMAGAMMA).error());
}


//  ***********************************************************************************************
//! @class PseudoVoigt2

//! to fit peak with a sum of Gaussian and Lorentzian with independent width parameters

class PseudoVoigt2 : public PeakFunction {
public:
    enum { parAMPL, parXSHIFT, parSIGMA, parGAMMA, parETA };

    PseudoVoigt2(
        double ampl = 1, double xShift = 0, double sigma = 1, double gamma = 1, double eta = 0.1);

    double y(double x, double const* P = nullptr) const final;
    double dy(double x, int parIndex, double const* P = nullptr) const final;

    void setGuessedPeak(const qpair&);
    void setGuessedFWHM(float);

    qpair fittedPeak() const;
    float fittedFWHM() const;

    qpair peakError() const;
    float fwhmError() const;

    QString name() const final { return "PseudoVoigt2"; }
};

PseudoVoigt2::PseudoVoigt2(double ampl, double mu, double hwhmG, double hwhmL, double eta)
{
    setParameterCount(5);

    auto& parAmpl = parameters_[parAMPL];
    auto& parMu = parameters_[parXSHIFT];
    auto& parHwhmG = parameters_[parSIGMA];
    auto& parHwhmL = parameters_[parGAMMA];
    auto& parEta = parameters_[parETA];

    parAmpl.setAllowedRange(0, Q_INFINITY);
    parAmpl.setValue(ampl, 0);

    parMu.setValue(mu, 0);

    parHwhmG.setAllowedRange(0, Q_INFINITY);
    parHwhmG.setValue(hwhmG, 0);

    parHwhmL.setAllowedRange(0, Q_INFINITY);
    parHwhmL.setValue(hwhmL, 0);

    parEta.setAllowedRange(0, 1);
    parEta.setValue(eta, 0);
}

double PseudoVoigt2::y(double x, double const* P) const {
    double ampl = parValue(parAMPL, P);
    double xShift = parValue(parXSHIFT, P);
    double sigma = parValue(parSIGMA, P);
    double gamma = parValue(parGAMMA, P);
    double eta = parValue(parETA, P);

    double argG = (x - xShift) / sigma;
    double argG2 = argG * argG;
    double gaussian = ampl * exp(-argG2 * log(2.0));

    double argL = (x - xShift) / gamma;
    double argL2 = argL * argL;
    double lorentz = ampl / (1 + argL2);

    return (1 - eta) * gaussian + eta * lorentz;
}

double PseudoVoigt2::dy(double x, int parIndex, double const* P) const
{
    double ampl = parValue(parAMPL, P);
    double xShift = parValue(parXSHIFT, P);
    double sigma = parValue(parSIGMA, P);
    double gamma = parValue(parGAMMA, P);
    double eta = parValue(parETA, P);

    double argG1 = (x - xShift) / sigma;
    double argG2 = argG1 * argG1;
    double argG3 = exp(-argG2 * log(2.0));

    double argL1 = (x - xShift) / gamma;
    double argL2 = argL1 * argL1;
    double argL3 = 1 + argL2;

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

void PseudoVoigt2::setGuessedPeak(const qpair& qpair)
{
    PeakFunction::setGuessedPeak(qpair);
    setParValue(parXSHIFT, qpair.x);
    setParValue(parAMPL, qpair.y);
}

void PseudoVoigt2::setGuessedFWHM(float fwhm)
{
    PeakFunction::setGuessedFWHM(fwhm);
    setParValue(parSIGMA, fwhm * 0.424661);
    setParValue(parGAMMA, fwhm / 2);
}

qpair PseudoVoigt2::fittedPeak() const
{
    return qpair(parameters_.at(parXSHIFT).value(), parameters_.at(parAMPL).value());
}

float PseudoVoigt2::fittedFWHM() const
{
    double eta = parameters_.at(parETA).value();
    return float(
        ((1 - eta) * parameters_.at(parSIGMA).value() / 0.424661
         + eta * parameters_.at(parGAMMA).value() * 2)
        / 2);
}

qpair PseudoVoigt2::peakError() const
{
    return qpair(parameters_.at(parXSHIFT).error(), parameters_.at(parAMPL).error());
}

float PseudoVoigt2::fwhmError() const
{
    // REVIEW
    return float(parameters_.at(parSIGMA).error() + parameters_.at(parGAMMA).error());
}

} // namespace


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
    initializer_type make_new = find_or_fail(peakFunctionName);
    return make_new();
}

PeakFunction* FunctionRegistry::clone(const PeakFunction& old)
{
    PeakFunction* ret = name2new(old.name());
    *ret = old;
    return ret;
}

FunctionRegistry::FunctionRegistry()
{
    register_fct([]()->PeakFunction*{return new Gaussian();});
    register_fct([]()->PeakFunction*{return new Lorentzian();});
    register_fct([]()->PeakFunction*{return new PseudoVoigt1();});
    register_fct([]()->PeakFunction*{return new PseudoVoigt2();});
}
*/

Fitted PeakFunction::fromFit(
    const QString& functionName, const Curve& curve, const RawOutcome& rawOutcome)
{
    ASSERT(curve.count());
    const PeakFunction* f;
    int nPar = 3;
    f = new Gaussian();
    std::vector<double> startParams(nPar);
    startParams[0] = rawOutcome.getCenter();
    startParams[1] = rawOutcome.getFwhm() / sqrt(8*log(2));
    startParams[2] = rawOutcome.getIntensity();
    return FitWrapper().execFit(f, curve, startParams);
}
