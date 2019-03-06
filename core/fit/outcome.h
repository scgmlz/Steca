//  ***********************************************************************************************
//
//  Steca: stress and texture calculator
//
//! @file      core/fit/outcome.h
//! @brief     Defines class PeakFunction
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
//  ***********************************************************************************************

#ifndef OUTCOME_H
#define OUTCOME_H

#include "core/fit/parametric_function.h"
#include <memory>

//! Numeric outcome of peak fit.

class PeakOutcome {
public:
    const DoubleWithError center;
    const DoubleWithError fwhm;
    const DoubleWithError intensity;
    const std::shared_ptr<DoubleWithError> gammOverSigma; // an optional value, it might NOT exist
};

#endif // OUTCOME_H
