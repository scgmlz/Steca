// ************************************************************************** //
//
//  Steca: stress and texture calculator
//
//! @file      core/calc/reflection.cpp
//! @brief     Implements class Reflection
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
// ************************************************************************** //

#include "core/calc/reflection.h"
#include "core/fit/fit_fun.h"
#include "core/def/idiomatic_for.h"
#include "core/session.h"

Reflection::Reflection(const QString& functionName) : peakFunction_(nullptr) {
    setPeakFunction(functionName);
}

PeakFunction const& Reflection::peakFunction() const {
    debug::ensure(peakFunction_);
    return *peakFunction_;
}

const Range& Reflection::range() const {
    return peakFunction_->range();
}

void Reflection::setRange(const Range& range) {
    peakFunction_->setRange(range);
}

void Reflection::invalidateGuesses() {
    peakFunction_->setGuessedPeak(qpair());
    peakFunction_->setGuessedFWHM(NAN);
}

void Reflection::setGuessPeak(qpair const& peak) {
    peakFunction_->setGuessedPeak(peak);
}

void Reflection::setGuessFWHM(fwhm_t fwhm) {
    peakFunction_->setGuessedFWHM(fwhm);
}

void Reflection::fit(Curve const& curve) {
    peakFunction_->fit(curve);
}

QString Reflection::functionName() const {
    return peakFunction_->name();
}

void Reflection::setPeakFunction(const QString& peakFunctionName) {
    bool haveRange = !peakFunction_.isNull();
    Range oldRange;
    if (haveRange)
        oldRange = peakFunction_->range();
    peakFunction_.reset(FunctionRegistry::name2new(peakFunctionName));
    if (haveRange)
        peakFunction_->setRange(oldRange);
}

JsonObj Reflection::to_json() const {
    return peakFunction_->to_json();
}

Reflection* Reflection::from_json(JsonObj const& obj) THROWS {
    str functionName = obj.loadString("type");
    Reflection* ret = new Reflection();
    ret->setPeakFunction(functionName);
    ret->peakFunction_->from_json(obj); // may throw
    return ret;
}


// ************************************************************************** //
//  class Peaks
// ************************************************************************** //

void Peaks::clear() {
    reflections_.clear();
}

void Peaks::add(const QString& functionName) {
    Reflection* reflection(new Reflection(functionName));
    debug::ensure(reflection);
    reflections_.push_back(reflection);
}

void Peaks::add(const QJsonObject& obj) {
    reflections_.push_back(Reflection::from_json(obj));
}

void Peaks::remove(int i) {
    delete reflections_[i];
    reflections_.erase(reflections_.begin()+i);
}

void Peaks::select(Reflection* reflection) {
    selected_ = reflection;
    emit gSession->sigReflectionSelected();
}

QStringList Peaks::names() const {
    QStringList ret;
    for_i (gSession->peaks().count())
        ret.append(QStringLiteral("%1: %2")
                   .arg(i+1)
                   .arg(reflections_[i]->functionName()));
    return ret;
}

QJsonArray Peaks::toJson() const {
    QJsonArray ret;
    for (auto& reflection : reflections_)
        ret.append(reflection->to_json());
    return ret;
}
