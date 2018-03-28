// ************************************************************************************************
//
//  Steca: stress and texture calculator
//
//! @file      core/algo/fitting.h
//! @brief     Defines functions rawFit, rawFits
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
// ************************************************************************************************

#ifndef FITTING_H
#define FITTING_H

#include "core/data/peak_info.h"

class ActiveClusters;
class Cluster;
class Peak;
class Progress;

//! @namespace algo
//! Core algorithms.

namespace algo {

PeakInfo rawFit(const Cluster&, const Peak&, const Range&);
PeakInfos rawFits(const ActiveClusters&, const Peak&, Progress*);

} // namespace algo

#endif // FITTING_H
