//  ***********************************************************************************************
//
//  Steca: stress and texture calculator
//
//! @file      core/algo/interpolate_polefig.h
//! @brief     Defines function interpolateInfos
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
//  ***********************************************************************************************

#ifndef INTERPOLATE_POLEFIG_H
#define INTERPOLATE_POLEFIG_H

class PeakInfos;

namespace algo {

PeakInfos&& interpolateInfos(const PeakInfos& direct);

} // namespace algo

#endif // INTERPOLATE_POLEFIG_H
