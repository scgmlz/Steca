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

#include "core/session.h"
#include "qcr/base/debug.h"

Peak::Peak(const Range& r, const QString& functionName)
    : range_(r)
    , peakFunction_(nullptr)
{
    setPeakFunction(functionName);
}

const PeakFunction& Peak::peakFunction() const
{
    ASSERT(peakFunction_);
    return *peakFunction_;
}

void Peak::setRange(const Range& r)
{
    range_ = r;
    invalidateGuesses();
    emit gSession->sigPeaks(); // TODO PeakPars(index())
}

void Peak::invalidateGuesses()
{
    peakFunction_->setGuessedPeak(qpair());
    peakFunction_->setGuessedFWHM(Q_QNAN);
}

void Peak::setGuessPeak(const qpair& peak)
{
    peakFunction_->setGuessedPeak(peak);
}

void Peak::setGuessFWHM(float fwhm)
{
    peakFunction_->setGuessedFWHM(fwhm);
}

void Peak::fit(const Curve& curve)
{
    peakFunction_->doFit(curve, range_);
}

void Peak::setPeakFunction(const QString& peakFunctionName)
{
    peakFunction_.reset(gSession->functionRegistry.name2new(peakFunctionName));
    emit gSession->sigPeaks(); // TODO PeakPars(index())
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
    emit gSession->sigPeaks(); // TODO PeakPars(index())
}


//  ***********************************************************************************************
//! @class Peaks

QString Peaks::defaultFunctionName = "Gaussian";

void Peaks::clear()
{
    peaks_.clear();
    emit gSession->sigPeaks();
}

void Peaks::add(const Range& range)
{
    peaks_.erase(std::remove_if(peaks_.begin(), peaks_.end(), [=](const Peak& p) {
                return p.range().intersects(range); }), peaks_.end());
    doAdd({range, defaultFunctionName});
    // not elegant: find the newly added range
    for (int i=0; i<count(); ++i) {
        if (at(i).range().intersects(range)) {
            selected_ = i;
            return;
        }
    }
    emit gSession->sigPeaks();
}

void Peaks::doAdd(Peak&& peak)
{
    peaks_.push_back(std::move(peak));
    sort();
    selected_ = count()-1;
}

void Peaks::removeSelected()
{
    ASSERT(0<=selected_ && selected_<count());
    peaks_.erase(peaks_.begin()+selected_);
    if (selected_>=count())
        selected_ = count()-1;
    emit gSession->sigPeaks();
}

void Peaks::selectByValue(double x)
{
    for (int i=0; i<count(); ++i) {
        if (at(i).range().contains(x)) {
            selected_ = i;
            return;
        }
    }
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
    emit gSession->sigPeaks();
}

void Peaks::sort()
{
    std::sort(peaks_.begin(), peaks_.end(), [](const Peak& p1, const Peak& p2) {
            return p1.range().min < p2.range().min; } );
}
