//  ***********************************************************************************************
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
//  ***********************************************************************************************

#include "core/session.h"
#include "core/algo/collect_intensities.h"
#include "core/data/cluster.h"
#include "qcr/base/debug.h"

ThetaSelection::ThetaSelection()
{
    currArc.setCoerce( [this](int i) { return qMax(0, qMin(i, numSlices_)); });
    currArc.setPostHook( [this](int) { recomputeCache(); /* UNDER CONSTRUCTION */ });
}


QJsonObject ThetaSelection::toJson() const
{
    return {
        { "current arc index", QJsonValue(currArc.val()) },
    };
}

void ThetaSelection::fromJson(const JsonObj& obj)
{
    currArc.setVal(obj.loadInt("current arc index"));
}

//! Resets fullRange_ and numSlices_ according to loaded data.
void ThetaSelection::onData()
{
    const Cluster* cluster = gSession->dataset().highlight().cluster();
    qDebug() << "ThetaSelection onData, highlighted cluster = " << cluster;
    if (!cluster) {
        fullRange_.invalidate();
        numSlices_ = 0;
        return;
    }
    fullRange_ = cluster->rgeTth();
    numSlices_ = algo::numTthBins(cluster->members(), fullRange_);
    recomputeCache();
}

void ThetaSelection::recomputeCache()
{
    if (!numSlices_)
        return;
    currArc.reCoerce();
    range_ = fullRange_.slice(currArc.val(), numSlices_);
}
