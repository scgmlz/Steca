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

#include "theta_selection.h"
#include "core/session.h"

ThetaSelection::ThetaSelection()
{
}

//! Resets fullRange_ and numSlices_ according to loaded data.
void ThetaSelection::onData()
{
    const Cluster* cluster = gSession->dataset().highlight().cluster();
    if (!cluster) {
        numSlices_ = 0;
        emit gSession->sigTheta();
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
