// ************************************************************************** //
//
//  Steca2: stress and texture calculator
//
//! @file      core/calc/calc_reflection.cpp
//! @brief     Implements class Reflection
//!
//! @homepage  https://github.com/scgmlz/Steca2
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2017
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
// ************************************************************************** //

#include "fit/fit_fun.h"
#include "calc_reflection.h"

namespace calc {

Reflection::Reflection(QString const& peakFunctionName) : peakFunction_(nullptr) {
    setPeakFunction(peakFunctionName);
}

PeakFunction const& Reflection::peakFunction() const {
    debug::ensure(peakFunction_);
    return *peakFunction_;
}

typ::Range const& Reflection::range() const {
    return peakFunction_->range();
}

void Reflection::setRange(typ::Range const& range) {
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

void Reflection::fit(typ::Curve const& curve) {
    peakFunction_->fit(curve);
}

QString Reflection::peakFunctionName() const {
    return peakFunction_->name();
}

void Reflection::setPeakFunction(QString const& peakFunctionName) {
    bool haveRange = !peakFunction_.isNull();
    typ::Range oldRange;
    if (haveRange)
        oldRange = peakFunction_->range();
    peakFunction_.reset(FunctionRegistry::name2new(peakFunctionName));
    if (haveRange)
        peakFunction_->setRange(oldRange);
}

typ::JsonObj Reflection::to_json() const {
    return peakFunction_->to_json();
}

void Reflection::from_json(typ::JsonObj const& obj) THROWS {
    str peakFunctionName = obj.loadString("type");
    setPeakFunction(peakFunctionName);
    peakFunction_->from_json(obj); // may throw
}

} // namespace calc
