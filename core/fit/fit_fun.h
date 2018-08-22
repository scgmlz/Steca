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
    double y(const double x) const;

    void setSuccess(bool s) { success_ = s; }
    void setParameterCount(int n) { parameters_.resize(n, {}); }
    FitParameter& parameterAt(int ip) { return parameters_[ip]; }

    int parameterCount() const { return parameters_.size(); }
    bool success() const { return success_; }

    virtual void setY(const double* parValues, const int nPts,
                      const double* xValues, double* yValues) const = 0;
    virtual void setDY(const double* parValues, const int nPar, const int nPts,
                       const double* xValues, double* jacobian) const = 0;

protected:
    void setParValue(int ip, double val) {parameters_[ip].setValue(val, 0); }

    std::vector<FitParameter> parameters_;
    bool success_ {false};
};


//! A polynomial, for fitting the background of a diffractogram

// TODO use Legendre polynomials on rescaled interval to provide an easy approximation of the average.
class Polynom : public ParametricFunction {
public:
    Polynom(int _degree) { setParameterCount(_degree + 1); } // only called by fromFit

    void setY(const double* parValues, const int nPts,
              const double* xValues, double* yValues) const final;
    void setDY(const double* parValues, const int nPar, const int nPts,
               const double* xValues, double* jacobian) const final;

    static Polynom fromFit(int degree, const Curve&, const Ranges&);
};


//! Abstract peak function

class PeakFunction : public ParametricFunction {
public:
    PeakFunction(const QString& functionName, const RawOutcome&);

    void setY(const double* parValues, const int nPts,
              const double* xValues, double* yValues) const override;
    void setDY(const double* parValues, const int nPar, const int nPts,
               const double* xValues, double* jacobian) const override;
    const FitParameter& getCenter() const;
    const FitParameter getFwhm() const;
    const FitParameter& getIntensity() const;

    static PeakFunction fromFit(const QString& functionName, const Curve&, const RawOutcome&);
};

#endif // FIT_FUN_H
