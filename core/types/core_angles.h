// ************************************************************************** //
//
//  STeCa2:    StressTexCalculator ver. 2
//
//! @file      core_angles.h
//! @brief     Coordinates
//!
//! @homepage  http://apps.jcns.fz-juelich.de/steca2
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016
//! @authors   Scientific Computing Group at MLZ Garching
//! @authors   Antti Soininen, Jan Burle, Rebecca Brydon
//! @authors   Based on the original STeCa by Christian Randau
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
  deg(qreal val = 0) : val_(val) {}
  operator qreal() const { return val_; }

  explicit deg(rad const&);
  rad toRad() const;

  deg normalized();

private:
  qreal val_;
};

/// trigonometry needs radians
class rad {
public:
  rad(qreal val = 0) : val_(val) {}
  operator qreal() const { return val_; }

  explicit rad(deg const&);
  deg toDeg() const;

private:
  qreal val_;
};

//------------------------------------------------------------------------------

class NormalizedDeg {
public:
  NormalizedDeg(deg angle = 0, bool force = false) { set(angle, force); }

  void set(deg angle, bool force = false) {
    angle_ = force ? angle : angle.normalized();
  }

  deg val() const { return angle_; }

  bool operator==(NormalizedDeg const& that) const {
    return angle_ == that.angle_;
  }

  bool operator<(NormalizedDeg const& that) const {
    return angle_ < that.angle_;
  }
  bool operator>(NormalizedDeg const& that) const {
    return angle_ > that.angle_;
  }
  bool operator<=(NormalizedDeg const& that) const {
    return angle_ <= that.angle_;
  }
  bool operator>=(NormalizedDeg const& that) const {
    return angle_ >= that.angle_;
  }

private:
  deg angle_;
};

//------------------------------------------------------------------------------
}
#endif  // CORE_ANGLES_H
