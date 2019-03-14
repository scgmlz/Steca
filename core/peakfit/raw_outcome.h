//  ***********************************************************************************************
//
//  Steca: stress and texture calculator
//
//! @file      core/peakfit/raw_outcome.h
//! @brief     Defines class RawOutcome
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
//  ***********************************************************************************************

#ifndef RAW_OUTCOME_H
#define RAW_OUTCOME_H

#include "core/peakfit/outcome.h"
#include <QtNumeric>

class Curve;

Mapped analyseRawPeak(const Curve&);

#endif // RAW_OUTCOME_H
