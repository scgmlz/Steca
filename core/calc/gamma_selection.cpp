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
    numSlices.setPostHook( [this](int) {selectSlice(iSlice_);} );
}

QJsonObject GammaSelection::toJson() const
{
    return {
        { "number of slices", QJsonValue(numSlices.val()) },
        { "current slice index", QJsonValue(iSlice_) },
    };
}

void GammaSelection::fromJson(const JsonObj& obj)
{
    numSlices.setParam(obj.loadInt("number of slices"));
    selectSlice(obj.loadInt("current slice index"));
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
        range_ = slice2range(iSlice_);
    EMITS("GammaSelection::recomputeCache", gSession->sigGamma());
}

void GammaSelection::selectSlice(int i)
{
    iSlice_ = qMin(qMax(i, 0), numSlices.val()-1);
    recomputeCache();
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
