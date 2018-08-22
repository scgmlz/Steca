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
#include <memory>

class Curve;
class RawOutcome;

class FitFunction {
public:
    virtual ~FitFunction() {}
    virtual void setY(const double* P, const int nXY, const double* X, double* Y) const = 0;
    virtual void setDY(const double* P, const int nXY, const double* X,
                       double* Jacobian) const = 0;
};

//! Abstract function with parameters

class ParametricFunction {
public:
    ParametricFunction(const int _nPar, const FitFunction* _f)
        : f{_f}, parameters_(_nPar) {}
    ~ParametricFunction() { delete f; }
    ParametricFunction(const ParametricFunction&) = delete;
    ParametricFunction(ParametricFunction&&) = default;

    void setSuccess(bool s) { success_ = s; }
    FitParameter& parameterAt(int ip) { return parameters_[ip]; }

    double y(const double x) const;
    int parameterCount() const { return parameters_.size(); }
    bool success() const { return success_; }
    const std::vector<FitParameter>& parameters() const { return parameters_; };

    const FitFunction* f;

private:
    std::vector<FitParameter> parameters_;
    bool success_ {false};
};


//! A polynomial, for fitting the background of a diffractogram

// TODO use Legendre polynomials on rescaled interval to provide an easy approximation of the average.
class Polynom : public FitFunction {
public:
    Polynom(int _degree) : nPar_{_degree + 1} {}
    void setY(const double* P, const int nXY, const double* X, double* Y) const final;
    void setDY(const double* P, const int nXY, const double* X, double* Jacobian) const final;
private:
    const int nPar_;
};


//! Abstract peak function

class PeakFunction : public FitFunction {
public:
    void setY(const double* P, const int nXY, const double* X, double* Y) const final;
    void setDY(const double* P, const int nXY, const double* X, double* Jacobian) const final;
    const FitParameter getCenter   (const std::vector<FitParameter>& par) const;
    const FitParameter getFwhm     (const std::vector<FitParameter>& par) const;
    const FitParameter getIntensity(const std::vector<FitParameter>& par) const;
};


// global functions

ParametricFunction peakfunctionFromFit(const QString&, const Curve&, const RawOutcome&);
ParametricFunction polynomFromFit(int degree, const Curve&, const Ranges&);

#endif // FIT_FUN_H
