// ************************************************************************** //
//
//  STeCa2:    StressTexCalculator ver. 2
//
//! @file      core_reflection.cpp
//!
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016
//! @authors   Scientific Computing Group at MLZ Garching
//! @authors   Original version: Christian Randau
//! @authors   Version 2: Antti Soininen, Jan Burle, Rebecca Brydon
//
// ************************************************************************** //

#include "core_reflection.h"
#include "types/core_json.h"
#include <QStringList>

namespace core {
//------------------------------------------------------------------------------

str_lst const& Reflection::typeStrLst() {
  static str_lst types {"Gaussian","Lorentzian","PseudoVoigt1","PseudoVoigt2"};
  return types;
}

rcstr Reflection::typeTag(ePeakType type) {
  return typeStrLst()[(int)type];
}

Reflection::Reflection(ePeakType type): peakFunction_(nullptr) {
  setPeakFunction(type);
  setRange(Range());
}

ePeakType Reflection::type() const {
  return peakFunction_->type();
}

void Reflection::setType(ePeakType type) {
  setPeakFunction(type);
}

void Reflection::setRange(rcRange range) {
  range_ = range;
}

fit::PeakFunction& Reflection::peakFunction() {
  ASSERT(peakFunction_)
  return *peakFunction_;
}

void Reflection::invalidateGuesses() {
  peakFunction_->setGuessedPeak(XY());
  peakFunction_->setGuessedFWHM(qQNaN());
}

void Reflection::setPeakFunction(ePeakType type) {
  setPeakFunction(fit::PeakFunction::factory(type));
}

void Reflection::setPeakFunction(fit::PeakFunction* f) {
  peakFunction_.reset(f);
}

static str const KEY_RANGE("range"), KEY_PEAK("peak");

JsonObj Reflection::saveJson() const {
  return JsonObj()
    .saveRange(KEY_RANGE, range_)
    .saveObj(KEY_PEAK, peakFunction_->saveJson());
}

void Reflection::loadJson(rcJsonObj obj) THROWS {
  range_ = obj.loadRange(KEY_RANGE);

  QScopedPointer<fit::Function> f(fit::Function::factory(obj.loadObj(KEY_PEAK)));

  RUNTIME_CHECK(dynamic_cast<fit::PeakFunction*>(f.data()),"must be a peak function");
  setPeakFunction(static_cast<fit::PeakFunction*>(f.take()));
}

//------------------------------------------------------------------------------
}
// eof
