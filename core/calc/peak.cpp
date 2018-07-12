//  ***********************************************************************************************
//
//  Steca: stress and texture calculator
//
//! @file      core/calc/peak.cpp
//! @brief     Implements classes Peak and Peaks
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
//  ***********************************************************************************************

#include "core/def/idiomatic_for.h"
#include "core/session.h"
#include "qcr/base/debug.h"

Peak::Peak(const Range& range, const QString& functionName)
    : peakFunction_(nullptr)
{
    setPeakFunction(functionName);
    peakFunction_->setRange(range);
}

const PeakFunction& Peak::peakFunction() const
{
    ASSERT(peakFunction_);
    return *peakFunction_;
}

void Peak::setRange(const Range& range)
{
    peakFunction_->setRange(range);
    invalidateGuesses();
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
    peakFunction_->fit(curve);
}

void Peak::setPeakFunction(const QString& peakFunctionName)
{
    bool haveRange = (bool)peakFunction_;
    Range oldRange;
    if (haveRange)
        oldRange = peakFunction_->fitRange();
    peakFunction_.reset(FunctionRegistry::name2new(peakFunctionName));
    if (haveRange)
        peakFunction_->setRange(oldRange);
}

JsonObj Peak::toJson() const
{
    QJsonObject ret;
    ret.insert("range", peakFunction_->fitRange().toJson() );
    ret.insert("type", peakFunction_->toJson());
    return ret;
}

Peak Peak::fromJson(const JsonObj& obj)
{
    Peak ret{obj.loadRange("range"), obj.loadString("type")};
    ret.peakFunction_->fromJson(obj); // may throw
    return ret;
}


//  ***********************************************************************************************
//! @class Peaks

QString Peaks::defaultFunctionName = "Raw"; // FunctionRegistry::instance()->keys()[0];

void Peaks::clear()
{
    peaks_.clear();
}

void Peaks::add(const Range& range)
{
    doAdd({range, defaultFunctionName});
}

void Peaks::doAdd(Peak&& peak)
{
    peaks_.push_back(std::move(peak));
    selected_ = count()-1;
}

void Peaks::removeSelected()
{
    ASSERT(0<=selected_ && selected_<count());
    peaks_.erase(peaks_.begin()+selected_);
    if (selected_>=count())
        selected_ = count()-1;
}

void Peaks::select(int i)
{
    selected_ = i;
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

QStringList Peaks::names() const
{
    QStringList ret;
    for_i (gSession->peaks().count())
        ret.append(QStringLiteral("%1: %2")
                   .arg(i+1)
                   .arg(peaks_[i].functionName()));
    return ret;
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
    for_i (arr.count())
        doAdd(Peak::fromJson(arr.at(i).toObject()));
}
