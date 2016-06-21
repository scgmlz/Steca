// ************************************************************************** //
//
//  STeCa2:    StressTextureCalculator ver. 2
//
//! @file      core_type_geometry.cpp
//!
//! @homepage  http://apps.jcns.fz-juelich.de/steca2
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016
//! @authors   Scientific Computing Group at MLZ Garching
//! @authors   Rebecca Brydon, Jan Burle,  Antti Soininen
//! @authors   Based on the original STeCa by Christian Randau
//
// ************************************************************************** //

#include "core_type_geometry.h"
#include <qmath.h>

namespace core {

//------------------------------------------------------------------------------

qreal const Geometry::MIN_DETECTOR_DISTANCE   = 10;
qreal const Geometry::MIN_DETECTOR_PIXEL_SIZE = .1;

Geometry::Geometry()
: detectorDistance(MIN_DETECTOR_DISTANCE), pixSize(MIN_DETECTOR_PIXEL_SIZE)
, isMidPixOffset(false), midPixOffset() {
}

int Geometry::compare(Geometry const& that) const {
  if (detectorDistance < that.detectorDistance) return -1;
  if (detectorDistance > that.detectorDistance) return +1;
  if (pixSize          < that.pixSize)          return -1;
  if (pixSize          > that.pixSize)          return +1;
  if (isMidPixOffset   < that.isMidPixOffset)   return -1;
  if (isMidPixOffset   > that.isMidPixOffset)   return +1;
  return midPixOffset.compare(that.midPixOffset);
}

//------------------------------------------------------------------------------

ImageCut::ImageCut() : ImageCut(0, 0, 0, 0) {}

ImageCut::ImageCut(uint left_, uint top_, uint right_, uint bottom_)
: left(left_), top(top_), right(right_), bottom(bottom_) {
}

int ImageCut::compare(const ImageCut& that) const {
  if (left   < that.left)   return -1;
  if (left   > that.left)   return +1;
  if (top    < that.top)    return -1;
  if (top    > that.top)    return +1;
  if (right  < that.right)  return -1;
  if (right  > that.right)  return +1;
  if (bottom < that.bottom) return -1;
  if (bottom > that.bottom) return +1;
  return 0;
}

size2d ImageCut::marginSize() const {
  return size2d(left + right, top + bottom);
}

//------------------------------------------------------------------------------

Angles::Angles() : Angles(0, 0) {}

Angles::Angles(deg gamma_, deg tth_) : gamma(gamma_), tth(tth_) {}

AngleMap::Key::Key(Geometry const& geometry_, size2d const& size_,
                   ImageCut const& cut_, IJ const& midPix_, deg midTth_)
  : geometry(geometry_), size(size_), cut(cut_), midPix(midPix_), midTth(midTth_) {
}

int AngleMap::Key::compare(AngleMap::Key const& that) const {
  int cmp;
  if ((cmp = geometry.compare(that.geometry)))  return cmp;
  if ((cmp = size.compare(that.size)))          return cmp;
  if ((cmp = cut.compare(that.cut)))            return cmp;
  if ((cmp = midPix.compare(that.midPix)))      return cmp;
  if (midTth < that.midTth) return -1;
  if (midTth > that.midTth) return +1;
  return 0;
}

//------------------------------------------------------------------------------

AngleMap::AngleMap(Key const& key) {
  calculate(key);
}

void AngleMap::calculate(Key const& key) {
  arrAngles_.fill(key.size);
  rgeGamma_.invalidate();
  rgeTth_.invalidate();

  if (key.size.isEmpty()) return;

  qreal pixSize = key.geometry.pixSize,
        detDist = key.geometry.detectorDistance;

  for_int (i, key.size.w) {
    qreal x       = (int(i) - key.midPix.i) * pixSize;
    rad   tthHorz = key.midTth.toRad() + atan(x / detDist);
    qreal h       = cos(tthHorz) * hypot(x, detDist);

    for_int (j, key.size.h) {
      qreal y          = (key.midPix.j - int(j)) * pixSize;
      qreal z          = hypot(x, y);
      qreal pixDetDist = hypot(z, detDist);
      rad   tth        = acos(h / pixDetDist);

      qreal r     = sqrt((pixDetDist * pixDetDist) - (h * h));
      rad   gamma = asin(y / r);

      if (tthHorz < 0) {
        tth   = -tth;
        gamma = -gamma;
      }

      arrAngles_.setAt(i, j, Angles(gamma.toDeg(), tth.toDeg()));
    }
  }

  for (uint i = key.cut.left, iEnd = key.size.w - key.cut.right; i < iEnd; ++i) {
    for (uint j = key.cut.top, jEnd = key.size.h - key.cut.bottom; j < jEnd; ++j) {
      auto& as = arrAngles_.at(i, j);
      rgeGamma_.extendBy(as.gamma);
      rgeTth_.extendBy(as.tth);
    }
  }
}

//------------------------------------------------------------------------------
}
// eof
