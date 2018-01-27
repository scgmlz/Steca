// ************************************************************************** //
//
//  Steca: stress and texture calculator
//
//! @file      core/calc/peak.cpp
//! @brief     Implements class Peak
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
// ************************************************************************** //

#include "core/calc/peak.h"
#include "core/fit/fit_fun.h"
#include "core/def/idiomatic_for.h"
#include "core/session.h"

Peak::Peak(const QString& functionName) : peakFunction_(nullptr) {
    setPeakFunction(functionName);
}

PeakFunction const& Peak::peakFunction() const {
    debug::ensure(peakFunction_);
    return *peakFunction_;
}

const Range& Peak::range() const {
    return peakFunction_->range();
}

void Peak::setRange(const Range& range) {
    peakFunction_->setRange(range);
}

void Peak::invalidateGuesses() {
    peakFunction_->setGuessedPeak(qpair());
    peakFunction_->setGuessedFWHM(NAN);
}

void Peak::setGuessPeak(qpair const& peak) {
    peakFunction_->setGuessedPeak(peak);
}

void Peak::setGuessFWHM(fwhm_t fwhm) {
    peakFunction_->setGuessedFWHM(fwhm);
}

void Peak::fit(Curve const& curve) {
    peakFunction_->fit(curve);
}

QString Peak::functionName() const {
    return peakFunction_->name();
}

void Peak::setPeakFunction(const QString& peakFunctionName) {
    bool haveRange = !peakFunction_.isNull();
    Range oldRange;
    if (haveRange)
        oldRange = peakFunction_->range();
    peakFunction_.reset(FunctionRegistry::name2new(peakFunctionName));
    if (haveRange)
        peakFunction_->setRange(oldRange);
}

JsonObj Peak::to_json() const {
    return peakFunction_->to_json();
}

Peak* Peak::from_json(JsonObj const& obj) THROWS {
    str functionName = obj.loadString("type");
    Peak* ret = new Peak();
    ret->setPeakFunction(functionName);
    ret->peakFunction_->from_json(obj); // may throw
    return ret;
}


// ************************************************************************** //
//  class Peaks
// ************************************************************************** //

void Peaks::clear() {
    peaks_.clear();
}

void Peaks::add(const QString& functionName) {
    Peak* peak(new Peak(functionName));
    debug::ensure(peak);
    add(peak);
}

void Peaks::add(const QJsonObject& obj) {
    add(Peak::from_json(obj));
}

void Peaks::add(Peak* peak) {
    peaks_.push_back(peak);
    selected_ = count()-1;
    emit gSession->sigPeaksChanged();
    emit gSession->sigPeakSelected();
}

void Peaks::remove() {
    debug::ensure(0<=selected_ && selected_<count());
    delete peaks_[selected_];
    peaks_.erase(peaks_.begin()+selected_);
    if (selected_>=count())
        selected_ = count()-1;
    emit gSession->sigPeaksChanged();
    emit gSession->sigPeakSelected();
}

void Peaks::select(int i) {
    debug::ensure(i<count());
    selected_ = i;
    emit gSession->sigPeakSelected();
}

QStringList Peaks::names() const {
    QStringList ret;
    for_i (gSession->peaks().count())
        ret.append(QStringLiteral("%1: %2")
                   .arg(i+1)
                   .arg(peaks_[i]->functionName()));
    return ret;
}

QJsonArray Peaks::toJson() const {
    QJsonArray ret;
    for (auto& peak : peaks_)
        ret.append(peak->to_json());
    return ret;
}
