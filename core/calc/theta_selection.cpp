// ************************************************************************** //
//
//  Steca: stress and texture calculator
//
//! @file      core/calc/theta_selection.cpp
//! @brief     Implements ThetaSelection
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
// ************************************************************************** //

#include "core/session.h"

ThetaSelection::ThetaSelection()
{
}

QJsonObject ThetaSelection::toJson() const
{
    return {
        { "number of slices", QJsonValue(numSlices_) },
        { "current slice index", QJsonValue(iSlice_) },
    };
}

void ThetaSelection::fromJson(const JsonObj& obj)
{
    numSlices_ = obj.loadInt("number of slices");
    selectSlice(obj.loadInt("current slice index"));
}

//! Resets fullRange_ and numSlices_ according to loaded data.
void ThetaSelection::onData()
{
    const Cluster* cluster = gSession->dataset().highlight().cluster();
    if (!cluster) {
        fullRange_.invalidate();
        numSlices_ = 0;
        return;
    }
    fullRange_ = cluster->rgeTth();
    numSlices_ = cluster->toCurve().count();
    recomputeCache();
}

void ThetaSelection::recomputeCache()
{
    if (!numSlices_)
        return;
    iSlice_ = qMin(qMax(iSlice_, 0), numSlices_-1);
    range_ = fullRange_.slice(iSlice_, numSlices_);
    emit gSession->sigTheta();
}

void ThetaSelection::selectSlice(int i)
{
    iSlice_ = i;
    recomputeCache();
}
