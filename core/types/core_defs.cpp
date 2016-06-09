// ************************************************************************** //
//
//  STeCa2:    StressTextureCalculator ver. 2
//
//! @file      core_defs.cpp
//!
//! @homepage  http://apps.jcns.fz-juelich.de/steca2
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016
//! @authors   Scientific Computing Group at MLZ Garching
//! @authors   Rebecca Brydon, Jan Burle,  Antti Soininen
//! @authors   Based on the original STeCa by Christian Randau
//
// ************************************************************************** //

#include "core_defs.h"

static_assert(QT_VERSION >= QT_VERSION_CHECK(5, 4, 0),
              "written for Qt >= 5.4, have " QT_VERSION_STR);

#ifdef QT_NO_EXCEPTIONS
#error needs exception handling
#endif

str const EMPTY_STR;

Exception::Exception() noexcept
: msg_(), msg8bit_(), silent_(true) {
}

Exception::Exception(rcstr msg) noexcept
: msg_(msg), msg8bit_(msg_.toLocal8Bit()), silent_(false) {
}

Exception::Exception(Exception const& that) noexcept
: thisClass(that.msg_) {
}

pcstr Exception::what() const noexcept {
  return msg8bit_.constData();
}

Exception* Exception::clone() const {
  return new Exception(*this);
}

void Exception::raise() const {
  throw *this;
}

// eof
