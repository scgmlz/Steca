//  ***********************************************************************************************
//
//  Steca: stress and texture calculator
//
//! @file      core/raw/angle_map.h
//! @brief     Defines classes ScatterDirection, AngleMap
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
//  ***********************************************************************************************

#ifndef ANGLE_MAP_H
#define ANGLE_MAP_H

#include "core/raw/geometry.h"
#include <QSharedPointer> // no auto rm

//! Holds (gamma, 2theta) for all pixels in a detector image, and caches sorted gamma values.

class AngleMap {
public:
    void recompute(const deg tth);

    const ScatterDirection& dirAt1(int i) const { return arrAngles_[i]; }
    const ScatterDirection& dirAt2(int ix, int iy) const { return dirAt1(pointToIndex(ix, iy)); }

    Range rgeTth() const { return rgeTth_; }
    Range rgeGma() const { return rgeGma_; }
    Range rgeGmaFull() const { return rgeGmaFull_; }

    void getGmaIndexes(const Range&, std::vector<int> const*&, int&, int&) const;

private:
    size2d size_;
    std::vector<ScatterDirection> arrAngles_;

    Range rgeTth_;
    Range rgeGma_, rgeGmaFull_;
    std::vector<deg> gmas_; //!< sorted gamma values
    std::vector<int> gmaIndexes_;

    int pointToIndex(int ix, int iy) const { return iy * size_.w + ix; }
};

#endif // ANGLE_MAP_H
