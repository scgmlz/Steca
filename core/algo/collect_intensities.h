//  ***********************************************************************************************
//
//  Steca: stress and texture calculator
//
//! @file      core/algo/collect_intensities.h
//! @brief     Defines functions projectIntensity
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
//  ***********************************************************************************************

#ifndef PROJECT_INTENSITY_H
#define PROJECT_INTENSITY_H

#include "core/typ/angles.h"
#include <QVector>

class Range;
class Measurement;
class Curve;

//! @namespace algo
//! Core algorithms.

namespace algo {

int numTthBins(const QVector<const Measurement*>& _members, const Range& _rgeTth);

Curve collectIntensities(const QVector<const Measurement*>& _members,
                         double _normFactor, const Range& _rgeGma, const Range& _rgeTth);

} // namespace algo

#endif // PROJECT_INTENSITY_H
