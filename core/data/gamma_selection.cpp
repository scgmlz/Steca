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
//#include "QCR/base/debug.h"

GammaSelection::GammaSelection()
{
    numSlices.setHook([](int){
        gSession->onDetector();
    });
}

QJsonObject GammaSelection::toJson() const
{
    return {
        { "number of slices", QJsonValue(numSlices.val()) },
        { "current slice index", QJsonValue(currSlice.val()) },
    };
}

const Range GammaSelection::currentRange() const
{
    const Cluster* cluster = gSession->currentCluster();
    if (!cluster)
        return {};

    const Range fullRange_ = cluster->rangeGma();

    if (!fullRange_.isValid())
        return {};
    else
        return slice2range(fullRange_, currSlice.val()-1);
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
}


Range GammaSelection::slice2range(const Range &fullRange, int i) const
{
    if (!numSlices.val())
        return {};
    return fullRange.slice(i, numSlices.val());
}
