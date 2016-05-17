// ************************************************************************** //
//
//  STeCa2:    StressTexCalculator ver. 2 REVIEW
//
//! @file      core_angles.h
//! @brief     Coordinates
//!
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016
//! @authors   Scientific Computing Group at MLZ Garching
//! @authors   Original version: Christian Randau
//! @authors   Version 2: Antti Soininen, Jan Burle, Rebecca Brydon
//
// ************************************************************************** //

#ifndef CORE_ANGLES_H
#define CORE_ANGLES_H

#include "core_defs.h"

namespace core {
//------------------------------------------------------------------------------

class rad;

/// angles stored in degrees
class deg {
public:
  deg(qreal val = 0): val_(val) {}
  operator qreal() const  { return val_; }

  explicit deg(rad const&);
  rad toRad() const;

private:
  qreal val_;
};

/// trigonometry needs radians
class rad {
public:
  rad(qreal val = 0): val_(val) {}
  operator qreal() const  { return val_; }

  explicit rad(deg const&);
  deg toDeg() const;

private:
  qreal val_;
};

//------------------------------------------------------------------------------

class NormalizedDeg {
public:
  NormalizedDeg(deg angle = 0, bool force = false) {
    set(angle,force);
  }

  void set(deg angle, bool force = false) {
    angle_ = force ? angle : normalize(angle);
  }

  deg val() const { return angle_; }

  static deg normalize(deg angle) {
    static qreal const MAX = 360;
    angle = fmod(angle,MAX);
    if (angle<0) angle = angle + MAX;
    return angle;
  }

  bool operator==(NormalizedDeg const& that) const { return angle_ == that.angle_; }

  bool operator <(NormalizedDeg const& that) const { return angle_  < that.angle_; }
  bool operator >(NormalizedDeg const& that) const { return angle_  > that.angle_; }
  bool operator<=(NormalizedDeg const& that) const { return angle_ <= that.angle_; }
  bool operator>=(NormalizedDeg const& that) const { return angle_ >= that.angle_; }

private:
  deg angle_;
};

//------------------------------------------------------------------------------
}
#endif // CORE_ANGLES_H
