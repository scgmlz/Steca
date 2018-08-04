//  ***********************************************************************************************
//
//  Steca: stress and texture calculator
//
//! @file      core/fit/fit_fun.h
//! @brief     Defines classes ParametricFunction, Polynom, PeakFunction
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

//! Abstract function with parameters

class ParametricFunction {
public:
    virtual ~ParametricFunction() {}

    //! evaluate the function y = f(x), with given (parValues) or own parameters
    virtual double y(double x, double const* parValues = nullptr) const = 0;
    //! partial derivative / parameter, with given (parValues) or own parameters
    virtual double dy(double x, int parIndex, double const* parValues = nullptr) const = 0;

    void setParameterCount(int n) { parameters_.resize(n, {}); }
    virtual void reset();
    FitParameter& parameterAt(int ip) { return parameters_[ip]; }

    int parameterCount() const { return parameters_.size(); }

protected:
    void setParValue(int ip, double val) {parameters_[ip].setValue(val, 0); }

    double parValue(int parIndex, double const* parValues) const;

    std::vector<FitParameter> parameters_;
};

//! A polynomial, for fitting the background of a diffractogram

// TODO use Legendre polynomials on rescaled interval to provide an easy approximation of the average.
class Polynom : public ParametricFunction {
public:
    Polynom(int _degree) { setParameterCount(_degree + 1); } // only called by fromFit

    double y(double x, double const* parValues = nullptr) const final;

    static Polynom fromFit(int degree, const Curve&, const Ranges&);

private:
    double dy(double x, int parIndex, double const* parValues = nullptr) const final;
};


//! Abstract peak function

class PeakFunction : public ParametricFunction {
public:
    PeakFunction();

    virtual void doFit(const Curve&, const Range&);
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

    qpair guessedPeak_;
    float guessedFWHM_;
};

#endif // FIT_FUN_H
