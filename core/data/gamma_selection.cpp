//  ***********************************************************************************************
//
//  Steca: stress and texture calculator
//
//! @file      core/data/gamma_selection.cpp
//! @brief     Implements GammaSelection
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
//  ***********************************************************************************************

#include "core/data/cluster.h"
#include "core/session.h"
//#include "qcr/base/debug.h"

GammaSelection::GammaSelection()
{
    numSlices.setHook([](int){gSession->onDetector();});
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
    numSlices.setVal(obj.loadInt("number of slices"));
    currSlice.setVal(obj.loadInt("current slice index"));
}

//! Resets fullRange_ according to loaded data.
void GammaSelection::onData()
{
    if (numSlices.val()>0) {
        if (currSlice.val()>numSlices.val())
            currSlice.setVal(numSlices.val());
        else if (currSlice.val()<1)
            currSlice.setVal(1);
    }
    const Cluster* cluster = gSession->currentCluster();
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
        range_ = slice2range(currSlice.val()-1);
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
