//  ***********************************************************************************************
//
//  Steca: stress and texture calculator
//
//! @file      core/data/theta_selection.h
//! @brief     Defines ThetaSelection
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
//  ***********************************************************************************************

#ifndef THETA_SELECTION_H
#define THETA_SELECTION_H

#include "qcr/engine/cell.h"

// TODO replace this entirely by synchronized range display in Dfgram and Image

//! Select a theta bin for overlay in scattering image.

class ThetaSelection {
public:
    void fromJson(const JsonObj& obj);
    void onData();

    QJsonObject toJson() const;
    const Range& range() const { return range_; }

    QcrCell<int> currArc {1}; // counting from 1, for user convenience

private:
    void recomputeCache();
    Range fullRange_;
    Range range_;
    int numSlices_;
};

#endif // THETA_SELECTION_H
