//  ***********************************************************************************************
//
//  Steca: stress and texture calculator
//
//! @file      core/fitengine/fit_methods.h
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

#include "core/fitengine/fitted.h"

//! Wraps a 3rd-party minimizer. Provides a single function: execFit.

//! Recommended usage: combine class instantiation and function call in one single statement,
//!
//!     Fitted outcome = FitWrapper().execFit(....);

class FitWrapper {
public:
    Fitted execFit(
        const FitFunction*, const class Curve&, std::vector<double> parValue,
        bool onlyPositiveParams = false);

private:
    // these pointers are valid during fit() call
    const FitFunction* f_;
    const std::vector<double>* X_ {nullptr};

    void callFit(double*, const double*, const double*, double*, int, const double*, int);

    void callbackY(double*, double*, int, int, void*);
    void callbackJacobianLM(double*, double*, int, int, void*);
};

#endif // FIT_METHODS_H
