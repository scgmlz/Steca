//  ***********************************************************************************************
//
//  Steca: stress and texture calculator
//
//! @file      core/pars/peak.cpp
//! @brief     Implements classes Peak and Peaks
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
//  ***********************************************************************************************

#include "core/pars/peak.h"
#include "core/session.h"
#include "qcr/base/debug.h"

const QStringList Peak::keys = { "Raw", "Gaussian", "Lorentzian" };

Peak::Peak(const Range& r, const QString& functionName)
    : range_(r)
    , functionName_(functionName)
{}

void Peak::setRange(const Range& r)
{
    range_ = r;
    gSession->onPeaks(); // TODO restrict to PeakAt(index())
}

void Peak::setOne(double val, bool namelyMax)
{
    range_.setOne(val, namelyMax);
    gSession->onPeaks(); // TODO restrict to PeakAt(index())
}

void Peak::setPeakFunction(const QString& name)
{
    functionName_ = name;
    gSession->onPeaks(); // TODO restrict to PeakAt(index())
}

JsonObj Peak::toJson() const
{
    QJsonObject ret;
    ret.insert("range", range_.toJson() );
    return ret;
}

Peak Peak::fromJson(const JsonObj& obj)
{
    return {obj.loadRange("range"), obj.loadString("type")};
    gSession->onPeaks(); // TODO PeakAt(index())
}


//  ***********************************************************************************************
//! @class Peaks

void Peaks::clear()
{
    peaks_.clear();
    gSession->onPeaks();
}

void Peaks::add(const Range& range)
{
    peaks_.erase(std::remove_if(peaks_.begin(), peaks_.end(), [=](const Peak& p) {
                return p.range().intersects(range); }), peaks_.end());
    doAdd({range, Peak::keys.at(gSession->params.defaultPeakFunction.val())});
    // not elegant: find the newly added range
    for (int i=0; i<size(); ++i) {
        if (at(i).range().intersects(range)) {
            selected_ = i;
            return;
        }
    }
    gSession->onPeaks();
}

void Peaks::doAdd(Peak&& peak)
{
    peaks_.push_back(std::move(peak));
    sort();
    selected_ = size()-1;
}

void Peaks::removeSelected()
{
    ASSERT(0<=selected_ && selected_<size());
    peaks_.erase(peaks_.begin()+selected_);
    if (selected_>=size())
        selected_ = size()-1;
    gSession->onPeaks();
}

//! Selects the range that contains x. If there is no such range, then selected_ is left unchanged.
//! Returns true if a range has been found else returns false.
bool Peaks::selectByValue(double x)
{
    for (int i=0; i<size(); ++i) {
        if (at(i).range().contains(x)) {
            selected_ = i;
            return true;
        }
    }
    return false;
}

QJsonArray Peaks::toJson() const
{
    QJsonArray ret;
    for (auto& peak : peaks_)
        ret.append(peak.toJson());
    return ret;
}

void Peaks::fromJson(const QJsonArray& arr)
{
    for (const auto& ele: arr)
        doAdd(Peak::fromJson(ele.toObject()));
    gSession->onPeaks();
}

void Peaks::sort()
{
    std::sort(peaks_.begin(), peaks_.end(), [](const Peak& p1, const Peak& p2) {
            return p1.range().min < p2.range().min; } );
}
