//  ***********************************************************************************************
//
//  Steca: stress and texture calculator
//
//! @file      core/fit/fit_par.h
//! @brief     Defines classes FitParameter
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
//  ***********************************************************************************************

#ifndef FIT_PAR_H
#define FIT_PAR_H

#include "core/fit/parametric_function.h"

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

#endif // FIT_PAR_H
