//  ***********************************************************************************************
//
//  Steca: stress and texture calculator
//
//! @file      core/calc/gamma_selection.cpp
//! @brief     Implements GammaSelection
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
//  ***********************************************************************************************

#include "core/session.h"
#include "core/data/cluster.h"
#include "qcr/engine/debug.h"

GammaSelection::GammaSelection()
{
    // TODO restore C<->human offset of 1 for current slice index?
    currSlice.setCoerce( [this](int i) { return qMax(0, qMin(i, numSlices.val())); });
    numSlices.setPostHook( [this](int) { currSlice.reCoerce(); });
    currSlice.setPostHook( [this](int) { recomputeCache(); });
}

QJsonObject GammaSelection::toJson() const
{
    return {
        { "number of slices", QJsonValue(numSlices.val()) },
        { "current slice index", QJsonValue(currSlice.val()) },
    };
}

void GammaSelection::fromJson(const JsonObj& obj)
{
    numSlices.setParam(obj.loadInt("number of slices"));
    currSlice.setParam(obj.loadInt("current slice index"));
}

//! Resets fullRange_ according to loaded data.
void GammaSelection::onData()
{
    const Cluster* cluster = gSession->dataset().highlight().cluster();
    qDebug() << "GammaSelection onData, highlighted cluster = " << cluster;
    if (!cluster)
        return fullRange_.invalidate();
    fullRange_ = cluster->rgeGma();
    recomputeCache();
}

//! Recomputes range_ and iSlice_.
void GammaSelection::recomputeCache()
{
    if (!fullRange_.isValid())
        range_.invalidate();
    else if (numSlices.val()==0)
        range_ = fullRange_;
    else
        range_ = slice2range(currSlice.val());
    EMITS("GammaSelection::recomputeCache", gSession->sigGamma());
}

void GammaSelection::setRange(const Range& r)
{
    range_ = r;
    recomputeCache();
}

Range GammaSelection::slice2range(int i) const
{
    if (!numSlices.val())
        return {};
    return fullRange_.slice(i, numSlices.val());
}
