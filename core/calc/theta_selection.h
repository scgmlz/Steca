//  ***********************************************************************************************
//
//  Steca: stress and texture calculator
//
//! @file      core/calc/theta_selection.h
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
#include <QObject>

//! Select a theta bin for overlay in scattering image.

class ThetaSelection : public QObject {
public:
    ThetaSelection();

    void fromJson(const JsonObj& obj);
    void onData();

    const Range& range() const { return range_; }
    QJsonObject toJson() const;

    ParamWrapper<int> currArc {"currArc", 0};

private:
    void recomputeCache();
    Range fullRange_;
    Range range_;
    int numSlices_;
};

#endif // THETA_SELECTION_H
