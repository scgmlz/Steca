//  ***********************************************************************************************
//
//  Steca: stress and texture calculator
//
//! @file      core/peakfit/fit_models.h
//! @brief     Defines fit models
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
//  ***********************************************************************************************

#ifndef FIT_MODELS_H
#define FIT_MODELS_H

#include "core/peakfit/peak_function.h"

//! A Gaussian as a peak fit function.

class Gaussian : public PeakFunction {
public:
    void setY(const double* P, const int nXY, const double* X, double* Y) const final;
    void setDY(const double* P, const int nXY, const double* X, double* Jacobian) const final;
};

//! A Lorentzian as a peak fit function.

class Lorentzian : public PeakFunction {
public:
    void setY(const double* P, const int nXY, const double* X, double* Y) const final;
    void setDY(const double* P, const int nXY, const double* X, double* Jacobian) const final;
};

//! A Voigt function is the convolution of a Gaussian and a Lorentzian.

class Voigt : public PeakFunction {
public:
    void setY(const double* P, const int nXY, const double* X, double* Y) const final;
    void setDY(const double* P, const int nXY, const double* X, double* Jacobian) const final;
    int nPar() const final { return 4; }
    PeakOutcome outcome(const Fitted&) const final;
};

#endif // FIT_MODELS_H
