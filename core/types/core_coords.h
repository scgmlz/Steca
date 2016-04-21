// ************************************************************************** //
//
//  STeCa2:    StressTexCalculator ver. 2
//
//! @file      core_coords.h
//! @brief     Coordinates
//!
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016
//! @authors   Scientific Computing Group at MLZ Garching
//! @authors   Original version: Christian Randau
//! @authors   Version 2: Antti Soininen, Jan Burle, Rebecca Brydon
//
// ************************************************************************** //

#ifndef CORE_COORDS_H
#define CORE_COORDS_H

#include "core_defs.h"
#include "core_types_fwd.h"

namespace core {
//------------------------------------------------------------------------------
/// 2D point, integers

struct IJ {
  int i, j;

  IJ();                  ///< (0,0)
  IJ(int, int);

  bool operator ==(IJ const&) const;

  JsonObj saveJson() const;
  void    loadJson(rcJsonObj) THROWS;
};

typedef IJ const& rcIJ;

//------------------------------------------------------------------------------
/// 2D point, reals

struct XY {
  qreal x, y;

  XY();                   ///< invalid (NaN)
  XY(qreal,qreal);

  void  invalidate();     ///< make invalid
  bool  isValid() const;  ///< is not NaN

  JsonObj saveJson() const;
  void    loadJson(rcJsonObj) THROWS;
};

typedef XY const& rcXY;

//------------------------------------------------------------------------------

template <typename F>
class NormalizedAngle {
public:
  NormalizedAngle(F angle = 0, bool force = false) {
    set(angle,force);
  }

  void set(F angle, bool force = false) {
    angle_ = force ? angle : normalize(angle);
  }

  F val() const { return angle_; }

  static F normalize(F angle) {
    static qreal const MAX = 360;
    angle = fmod(angle,MAX);
    if (angle<0) angle += MAX;
    return angle;
  }

  bool operator==(NormalizedAngle const& that) const { return angle_ == that.angle_; }

  bool operator <(NormalizedAngle const& that) const { return angle_  < that.angle_; }
  bool operator >(NormalizedAngle const& that) const { return angle_  > that.angle_; }
  bool operator<=(NormalizedAngle const& that) const { return angle_ <= that.angle_; }
  bool operator>=(NormalizedAngle const& that) const { return angle_ >= that.angle_; }

private:
  F angle_;
};

//------------------------------------------------------------------------------
}
#endif // CORE_COORDS_H
