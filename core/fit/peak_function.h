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

class RawOutcome;

//! Numeric outcome of peak fit.

class PeakOutcome {
public:
    const DoubleWithError center;
    const DoubleWithError fwhm;
    const DoubleWithError intensity;
};

//! Abstract peak function

class PeakFunction : public FitFunction {
public:
    virtual PeakOutcome outcome(const Fitted&) const;

    static Fitted fromFit(const QString&, const Curve&, const RawOutcome&);
};

#endif // PEAK_FUNCTION_H
