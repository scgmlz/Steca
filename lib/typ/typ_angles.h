// ************************************************************************** //
//
//  STeCa2:    StressTextureCalculator ver. 2
//
//! @file      typ_angles.h
//! @brief     degrees and radians
//!
//! @homepage  http://apps.jcns.fz-juelich.de/steca2
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016
//! @authors   Scientific Computing Group at MLZ Garching
//! @authors   Rebecca Brydon, Jan Burle,  Antti Soininen
//! @authors   Based on the original STeCa by Christian Randau
//
// ************************************************************************** //

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

private:
  qreal val_;
};

//------------------------------------------------------------------------------
}
#endif // TYP_ANGLES_H
