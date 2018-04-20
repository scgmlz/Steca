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

class QProgressBar;
class Curve;
class Measurement;
class Range;
class Sequence;

//! @namespace algo
//! Core algorithms.

namespace algo {

int numTthBins(const std::vector<const Measurement*>&, const Range&);
Curve projectCluster(const Sequence&, const Range&);
void projectActiveClusters(QProgressBar*);

} // namespace algo

#endif // PROJECT_INTENSITY_H
