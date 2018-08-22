//  ***********************************************************************************************
//
//  Steca: stress and texture calculator
//
//! @file      core/fit/polynom.h
//! @brief     Defines class Polynom
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
//  ***********************************************************************************************

#ifndef POLYNOM_H
#define POLYNOM_H

#include "core/fit/parametric_function.h"
#include "core/typ/range.h"

//! A polynomial, for fitting the background of a diffractogram

// TODO use Legendre polynomials on rescaled interval to provide an easy approximation of the average.
class Polynom : public FitFunction {
public:
    Polynom(int _degree) : nPar_{_degree + 1} {}

    void setY(const double* P, const int nXY, const double* X, double* Y) const final;
    void setDY(const double* P, const int nXY, const double* X, double* Jacobian) const final;
    int nPar() const final { return nPar_; };

    static Fitted fromFit(int degree, const Curve&, const Ranges&);

private:
    const int nPar_;
};

#endif // POLYNOM_H
