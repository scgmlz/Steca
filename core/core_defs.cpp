// ************************************************************************** //
//
//  STeCa2:    StressTexCalculator ver. 2
//
//! @file      core_defs.cpp
//!
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016
//! @authors   Scientific Computing Group at MLZ Garching
//! @authors   Original version: Christian Randau
//! @authors   Version 2: Antti Soininen, Jan Burle, Rebecca Brydon
//
// ************************************************************************** //

#include "core_defs.h"
#include <QtMath>

static_assert (QT_VERSION >= QT_VERSION_CHECK(5,4,0),"written for Qt >= 5.4, have " QT_VERSION_STR);

qreal radToDeg(qreal rad) {
  return rad * (180 / M_PI);
}

qreal degToRad(qreal deg) {
  return deg * (M_PI / 180);
}

#ifdef QT_NO_EXCEPTIONS
#error needs exception handling
#endif

str const EMPTY_STR;

Exception* Exception::clone() const {
  return new Exception(*this);
}

void Exception::raise() const {
  throw *this;
}

// eof
