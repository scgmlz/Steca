// ************************************************************************** //
//
//  Steca2: stress and texture calculator
//
//! @file      core/calc/calc_reflection.cpp
//! @brief     Implements ...
//!
//! @homepage  https://github.com/scgmlz/Steca2
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2017
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
// ************************************************************************** //

#include "calc_reflection.h"

namespace calc {

str_lst::rc Reflection::typeStrLst() {
    static str_lst types{ "Raw", "Gaussian", "Lorentzian", "PseudoVoigt1", "PseudoVoigt2" };
    return types;
}

rcstr Reflection::typeTag(fit::ePeakType type) {
    return typeStrLst().at(uint(type));
}

Reflection::Reflection(fit::ePeakType type) : peakFunction_(nullptr) {
    setPeakFunction(type);
}

fit::ePeakType Reflection::type() const {
    return peakFunction_->type();
}

void Reflection::setType(fit::ePeakType type) {
    setPeakFunction(type);
}

fit::PeakFunction::rc Reflection::peakFunction() const {
    ENSURE(peakFunction_)
    return *peakFunction_;
}

typ::Range::rc Reflection::range() const {
    return peakFunction_->range();
}

void Reflection::setRange(typ::Range::rc range) {
    peakFunction_->setRange(range);
}

void Reflection::invalidateGuesses() {
    peakFunction_->setGuessedPeak(qpair());
    peakFunction_->setGuessedFWHM(NAN);
}

void Reflection::fit(typ::Curve::rc curve) {
    peakFunction_->fit(curve);
}

void Reflection::setPeakFunction(fit::ePeakType type) {
    bool haveRange = !peakFunction_.isNull();
    typ::Range oldRange;
    if (haveRange)
        oldRange = peakFunction_->range();

    setPeakFunction(fit::PeakFunction::factory(type));

    if (haveRange)
        peakFunction_->setRange(oldRange);
}

void Reflection::setPeakFunction(fit::PeakFunction* f) {
    peakFunction_.reset(f);
}

typ::JsonObj Reflection::saveJson() const {
    return peakFunction_->saveJson();
}

void Reflection::loadJson(typ::JsonObj::rc obj) THROWS {
    scoped<typ::Function*> f(typ::Function::make(obj));

    RUNTIME_CHECK(dynamic_cast<fit::PeakFunction*>(f.ptr()), "must be a peak function");
    setPeakFunction(static_cast<fit::PeakFunction*>(f.take()));
}
}
