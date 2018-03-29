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

QJsonObject GammaSelection::toJson() const
{
    return {
        { "number of slices", QJsonValue(numSlices_) },
        { "current slice index", QJsonValue(iSlice_) },
    };
}

void GammaSelection::fromJson(const JsonObj& obj)
{
    setNumSlices(obj.loadInt("number of slices"));
    selectSlice(obj.loadInt("current slice index"));
}

//! Resets fullRange_ according to loaded data.
void GammaSelection::onData()
{
    const Cluster* cluster = gSession->dataset().highlight().cluster();
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
    else if (numSlices_==0)
        range_ = fullRange_;
    else
        range_ = slice2range(iSlice_);
    EMIT(gSession->sigGamma());
}

void GammaSelection::setNumSlices(int n)
{
    numSlices_ = qMax(0, n);
    selectSlice(iSlice_);
}

void GammaSelection::selectSlice(int i)
{
    iSlice_ = qMin(qMax(i, 0), numSlices_-1);
    recomputeCache();
}

void GammaSelection::setRange(const Range& r)
{
    range_ = r;
    recomputeCache();
}

Range GammaSelection::slice2range(int i) const
{
    if (!numSlices_)
        return {};
    return fullRange_.slice(i, numSlices_);
}
