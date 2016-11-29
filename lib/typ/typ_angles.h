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

#ifndef TYP_ANGLES_H
#define TYP_ANGLES_H

#include "def/def_macros.h"

namespace typ {
//------------------------------------------------------------------------------

class rad;

// angles stored in degrees
class deg {
public:
  deg(qreal val = 0) : val_(val) {}
  operator qreal() const { return val_; }

  explicit deg(rad);
  rad toRad() const;

  deg& operator+=(deg const&);
  deg& operator*=(qreal);

  deg normalized();

private:
  qreal val_;
};

// trigonometry needs radians
class rad {
public:
  rad(qreal val = 0) : val_(val) {}
  operator qreal() const { return val_; }

  explicit rad(deg);
  deg toDeg() const;

  rad& operator+=(rad const&);
  rad& operator*=(qreal);

private:
  qreal val_;
};

//------------------------------------------------------------------------------
}
#endif // TYP_ANGLES_H
