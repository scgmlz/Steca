//  ***********************************************************************************************
//
//  Steca: stress and texture calculator
//
//! @file      core/pars/allpeaks_settings.cpp
//! @brief     Implements class Peaks
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
//  ***********************************************************************************************

#include "core/pars/allpeaks_settings.h"
#include "core/session.h"
#include "core/base/exception.h"
#include "qcr/base/debug.h"

void AllPeaksSettings::clear()
{
    peaksSettings_.clear();
    gSession->onPeaks();
}

void AllPeaksSettings::add(const Range& range)
{
    peaksSettings_.erase(std::remove_if(peaksSettings_.begin(), peaksSettings_.end(), [=](const OnePeakSettings& p) {
                return p.range().intersects(range); }), peaksSettings_.end());
    doAdd({range, OnePeakSettings::functionNames.at(gSession->params.defaultPeakFunction.val())});
    // not elegant: find the newly added range
    for (int i=0; i<size(); ++i) {
        if (at(i).range().intersects(range)) {
            selected_ = i;
            return;
        }
    }
    gSession->onPeaks();
}

void AllPeaksSettings::doAdd(OnePeakSettings&& peak)
{
    peaksSettings_.push_back(std::move(peak));
    sort();
    selected_ = size()-1;
}

void AllPeaksSettings::removeSelected()
{
    ASSERT(0<=selected_ && selected_<size());
    peaksSettings_.erase(peaksSettings_.begin()+selected_);
    if (selected_>=size())
        selected_ = size()-1;
    gSession->onPeaks();
}

//! Selects the range that contains x. If there is no such range, then selected_ is left unchanged.
//! Returns true if a range has been found else returns false.
bool AllPeaksSettings::selectByValue(double x)
{
    for (int i=0; i<size(); ++i) {
        if (at(i).range().contains(x)) {
            selected_ = i;
            return true;
        }
    }
    return false;
}

QJsonArray AllPeaksSettings::toJson() const
{
    QJsonArray ret;
    for (auto& peak : peaksSettings_)
        ret.append(peak.toJson());
    return ret;
}

void AllPeaksSettings::fromJson(const QJsonArray& arr)
{
    for (const auto& ele: arr)
        doAdd(OnePeakSettings::fromJson(ele.toObject()));
    gSession->onPeaks();
}

void AllPeaksSettings::sort()
{
    std::sort(peaksSettings_.begin(), peaksSettings_.end(),
              [](const OnePeakSettings& p1, const OnePeakSettings& p2) {
                  return p1.range().min < p2.range().min; } );
}
