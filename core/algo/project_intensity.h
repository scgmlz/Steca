//  ***********************************************************************************************
//
//  Steca: stress and texture calculator
//
//! @file      core/algo/project_intensity.h
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

//! @namespace algo
//! Core algorithms.

namespace algo {

void projectIntensity(
    QVector<float>& intens, QVector<int>& counts,
    const Measurement& measurement, const Range& rgeGma, deg minTth, deg deltaTth);

} // namespace algo

#endif // PROJECT_INTENSITY_H
