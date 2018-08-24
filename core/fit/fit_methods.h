//  ***********************************************************************************************
//
//  Steca: stress and texture calculator
//
//! @file      core/fit/fit_methods.h
//! @brief     Defines class FitWrapper
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
//  ***********************************************************************************************

#ifndef FIT_METHODS_H
#define FIT_METHODS_H

#include "core/fit/parametric_function.h"

//! Wraps Levenberg-Marquardt fit function from 3rd-party library.

//! Used for fitting polynomial background and peak functions.

class FitWrapper {
public:
    Fitted execFit(
        const FitFunction*, const class Curve&, std::vector<double> parValue);

private:
    // these pointers are valid during fit() call
    const FitFunction* f_;
    const std::vector<double>* X_ {nullptr};

    void callFit(double*, double const*, double const*, double*, int, double const*, int);

    void callbackY(double*, double*, int, int, void*);
    void callbackJacobianLM(double*, double*, int, int, void*);
};

#endif // FIT_METHODS_H
