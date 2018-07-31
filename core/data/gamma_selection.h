//  ***********************************************************************************************
//
//  Steca: stress and texture calculator
//
//! @file      core/data/gamma_selection.h
//! @brief     Defines GammaSelection
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
//  ***********************************************************************************************

#ifndef GAMMA_SELECTION_H
#define GAMMA_SELECTION_H

#include "qcr/engine/param_wrapper.h"
#include <QObject>

//! Supports different ways of setting a gamma range.

class GammaSelection : public QObject {
public:
    GammaSelection();

    void fromJson(const JsonObj& obj);
    void setRange(const Range&);
    void onData();

    QJsonObject toJson() const;
    const Range& range() const { return range_; }
    Range slice2range(int) const; // TODO NOW rm
    double min() const { return range_.min; }// TODO NOW rm
    double max() const { return range_.max; }// TODO NOW rm

    ParamWrapper<int> numSlices {1};
    ParamWrapper<int> currSlice {1}; // counting from 1, for user convenience

private:
    void recomputeCache();// TODO NOW rm
    Range fullRange_;// TODO NOW rm
    Range range_;// TODO NOW rm
};

#endif // GAMMA_SELECTION_H
