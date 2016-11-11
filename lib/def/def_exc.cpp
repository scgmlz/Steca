// ************************************************************************** //
//
//  STeCa2:    StressTextureCalculator ver. 2
//
//! @file      def_exc.cpp
//!
//! @homepage  http://apps.jcns.fz-juelich.de/steca2
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016
//! @authors   Scientific Computing Group at MLZ Garching
//! @authors   Rebecca Brydon, Jan Burle, Antti Soininen
//! @authors   Based on the original STeCa by Christian Randau
//
// ************************************************************************** //

#include "def_exc.h"

#ifdef QT_NO_EXCEPTIONS
#error needs exception handling
#endif

Exception::Exception(rcstr msg, bool silent) noexcept
: silent_(silent) {
  setMsg(msg);
}

Exception::Exception() noexcept
: Cls(EMPTY_STR, true) {
}

Exception::Exception(rcstr msg) noexcept
: Cls(msg, false) {
}

Exception::Exception(rc that) noexcept
: Cls(that.msg_) {
}

pcstr Exception::what() const noexcept {
  return msg8bit_.constData();
}

void Exception::setMsg(rcstr s) {
  msg_     = s;
  msg8bit_ = msg_.toLocal8Bit();
}

Exception* Exception::clone() const {
  return new Exception(*this);
}

void Exception::raise() const {
  throw *this;
}

// eof
