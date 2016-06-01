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
//! @authors   Antti Soininen, Jan Burle, Rebecca Brydon
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

bool Geometry::operator==(Geometry const& that) const {
  return detectorDistance == that.detectorDistance && pixSize == that.pixSize &&
         isMidPixOffset == that.isMidPixOffset &&
         midPixOffset == that.midPixOffset;
}

//------------------------------------------------------------------------------

ImageCut::ImageCut() : ImageCut(0, 0, 0, 0) {}

ImageCut::ImageCut(uint left_, uint top_, uint right_, uint bottom_)
: left(left_), top(top_), right(right_), bottom(bottom_) {
}

bool ImageCut::operator==(const ImageCut& that) const {
  return (left == that.left && top == that.top && right == that.right &&
          bottom == that.bottom);
}

QSize ImageCut::marginSize() const {
  return QSize(left + right, top + bottom);
}

//------------------------------------------------------------------------------

Angles::Angles() : Angles(0, 0) {}

Angles::Angles(deg gamma_, deg tth_) : gamma(gamma_), tth(tth_) {}

//------------------------------------------------------------------------------

AngleMap::AngleMap() {}

void AngleMap::calculate(deg midTth, Geometry const& geometry,
                         QSize const& size, ImageCut const& cut, rcIJ midPix) {
  arrAngles_.fill(size);
  rgeGamma_.invalidate();
  rgeTth_.invalidate();

  if (size.isEmpty()) return;

  qreal pixSize = geometry.pixSize, detDist = geometry.detectorDistance;

  for_int (i, size.width()) {
    qreal x       = (i - midPix.i) * pixSize;
    rad   tthHorz = midTth.toRad() + atan(x / detDist);
    qreal h       = cos(tthHorz) * hypot(x, detDist);

    for_int (j, size.height()) {
      qreal y          = -(j - midPix.j) * pixSize;
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

  for (int i = cut.left, iEnd = size.width() - cut.right; i < iEnd; ++i) {
    for (int j = cut.top, jEnd = size.height() - cut.bottom; j < jEnd; ++j) {
      auto& as = arrAngles_.at(i, j);
      rgeGamma_.extendBy(as.gamma);
      rgeTth_.extendBy(as.tth);
    }
  }
}

//------------------------------------------------------------------------------
}
// eof
