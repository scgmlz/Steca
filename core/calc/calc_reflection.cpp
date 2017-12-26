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

#include "calc_reflection.h"

namespace calc {

QStringList const& Reflection::typeStrLst() {
    static QStringList types{ "Raw", "Gaussian", "Lorentzian", "PseudoVoigt1", "PseudoVoigt2" };
    return types;
}

Reflection::Reflection(fit::ePeakType type) : peakFunction_(nullptr) {
    setPeakFunction(type);
}

void Reflection::setPeakTypeIndex(uint index) {
    setPeakFunction(fit::ePeakType(index));
}

fit::PeakFunction const& Reflection::peakFunction() const {
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

void Reflection::fit(typ::Curve const& curve) {
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

typ::JsonObj Reflection::to_json() const {
    return peakFunction_->to_json();
}

void Reflection::from_json(typ::JsonObj const& obj) THROWS {
    scoped<typ::Function*> f(typ::Function::make(obj));
    RUNTIME_CHECK(dynamic_cast<fit::PeakFunction*>(f.ptr()), "must be a peak function");
    setPeakFunction(static_cast<fit::PeakFunction*>(f.take()));
}

} // namespace calc
