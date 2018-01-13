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

Reflection::Reflection(const QString& peakFunctionName) : peakFunction_(nullptr) {
    setPeakFunction(peakFunctionName);
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

QString Reflection::peakFunctionName() const {
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

void Reflection::from_json(JsonObj const& obj) THROWS {
    str peakFunctionName = obj.loadString("type");
    setPeakFunction(peakFunctionName);
    peakFunction_->from_json(obj); // may throw
}


QStringList reflectionNames(const Reflections& reflections) {
    QStringList ret;
    for_i (reflections.count())
        ret.append(QStringLiteral("%1: %2").arg(i+1).arg(reflections[i]->peakFunction().name()));
    return ret;
}
