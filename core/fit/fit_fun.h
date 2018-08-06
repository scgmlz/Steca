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

#include "core/fit/fit_par.h"

class Curve;
class RawOutcome;

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
    PeakFunction(const QString& functionName, const RawOutcome&);
    double y(double x, double const* parValues = nullptr) const final;
    const FitParameter& getCenter() const;
    const FitParameter getFwhm() const;
    const FitParameter& getIntensity() const;
    static PeakFunction fromFit(const QString& functionName, const Curve&, const RawOutcome&);

private:
    double dy(double x, int parIndex, double const* parValues = nullptr) const final;
};

#endif // FIT_FUN_H
