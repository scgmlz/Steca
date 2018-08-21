//  ***********************************************************************************************
//
//  Steca: stress and texture calculator
//
//! @file      core/data/collect_intensities.h
//! @brief     Defines functions projectIntensity
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
//  ***********************************************************************************************

#ifndef COLLECT_INTENSITIES_H
#define COLLECT_INTENSITIES_H

#include <vector> // no auto rm

class Curve;
class Measurement;
class Range;
class Sequence;

//! @namespace algo
//! Core algorithms.

namespace algo {

int numTthBins(const std::vector<const Measurement*>&, const Range&);
Curve projectCluster(const Sequence&, const Range&);

} // namespace algo

#endif // COLLECT_INTENSITIES_H
