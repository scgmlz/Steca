//  ***********************************************************************************************
//
//  Steca: stress and texture calculator
//
//! @file      core/fit/peak_function.h
//! @brief     Defines class PeakFunction
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
//  ***********************************************************************************************

#ifndef PEAK_FUNCTION_H
#define PEAK_FUNCTION_H

#include "core/fit/parametric_function.h"
#include <QString>

//! Abstract peak function

class PeakFunction : public FitFunction {
public:
    void setY(const double* P, const int nXY, const double* X, double* Y) const final;
    void setDY(const double* P, const int nXY, const double* X, double* Jacobian) const final;

    const FitParameter getCenter   (const std::vector<FitParameter>& par) const;
    const FitParameter getFwhm     (const std::vector<FitParameter>& par) const;
    const FitParameter getIntensity(const std::vector<FitParameter>& par) const;

    static ParametricFunction fromFit(const QString&, const Curve&, const RawOutcome&);
};

#endif // PEAK_FUNCTION_H
