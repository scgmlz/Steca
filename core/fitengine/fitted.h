//  ***********************************************************************************************
//
//  Steca: stress and texture calculator
//
//! @file      core/fitengine/fitted.h
//! @brief     Defines class Fitted
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
//  ***********************************************************************************************

#ifndef FITTED_H
#define FITTED_H

#include "core/fitengine/fit_function.h"
#include <memory>
#include <vector>

class Curve;

//! The outcome of a fit: a function, some fitted parameters, and a success flag.

class Fitted {
public:
    Fitted() {}                                 //!< When fit has failed.
    Fitted(const FitFunction* _f,
           const std::vector<double>& _parVal,
           const std::vector<double>& _parErr); //!< To hold outcome of successful fit
    Fitted(const Fitted&) = delete;
    Fitted(Fitted&&) = default;

    bool success() const { return success_; }
    int nPar() const { return f_->nPar(); }
    double parValAt(int i) const { return parVal_.at(i); }
    double parErrAt(int i) const { return parErr_.at(i); }
    double y(const double x) const;
    const FitFunction* fitFunction() const { return f_.get(); }

private:
    const bool success_ {false};
    std::unique_ptr<const FitFunction> f_;
    const std::vector<double> parVal_;
    const std::vector<double> parErr_;
};

#endif // FITTED_H
