/*******************************************************************************
 * STeCa2 - StressTextureCalculator ver. 2
 *
 * Copyright (C) 2016 Forschungszentrum Jülich GmbH 2016
 *
 * This program is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the Free
 * Software Foundation, either version 3 of the License, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE.
 *
 * See the COPYING and AUTHORS files for more details.
 ******************************************************************************/

#include "typ_angles.h"
#include <qmath.h>

namespace typ {
//------------------------------------------------------------------------------

deg::deg(rad r) {
  val_ = r.toDeg();
}

rad deg::toRad() const {
  return val_ * (M_PI / 180);
}

deg& deg::operator+=(deg const& that) {
  val_ += that.val_;
  return *this;
}

deg& deg::operator*=(qreal fac) {
  val_ *= fac;
  return *this;
}

deg deg::normalized() {
  static qreal const MAX = 360;
  qreal norm = fmod(val_, MAX);
  if (norm < 0) norm += MAX;
  return norm;
}

//------------------------------------------------------------------------------

rad::rad(deg d) {
  val_ = d.toRad();
}

deg rad::toDeg() const {
  return val_ * (180 / M_PI);
}

rad& rad::operator+=(rad const& that) {
  val_ += that.val_;
  return *this;
}

rad& rad::operator*=(qreal fac) {
  val_ *= fac;
  return *this;
}

//------------------------------------------------------------------------------
}
// eof
