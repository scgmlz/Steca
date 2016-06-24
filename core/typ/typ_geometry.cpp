// ************************************************************************** //
//
//  STeCa2:    StressTextureCalculator ver. 2
//
//! @file      typ_geometry.cpp
//!
//! @homepage  http://apps.jcns.fz-juelich.de/steca2
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016
//! @authors   Scientific Computing Group at MLZ Garching
//! @authors   Rebecca Brydon, Jan Burle,  Antti Soininen
//! @authors   Based on the original STeCa by Christian Randau
//
// ************************************************************************** //

#include "typ_geometry.h"
#include "def/def_compare.h"
#include <qmath.h>

namespace typ {
//------------------------------------------------------------------------------

qreal const Geometry::MIN_DETECTOR_DISTANCE   = 10;
qreal const Geometry::MIN_DETECTOR_PIXEL_SIZE = .1;

Geometry::Geometry()
: detectorDistance(MIN_DETECTOR_DISTANCE), pixSize(MIN_DETECTOR_PIXEL_SIZE)
, isMidPixOffset(false), midPixOffset() {
}

int Geometry::compare(rc that) const {
  COMPARE_VALUE(detectorDistance)
  COMPARE_VALUE(pixSize)
  COMPARE_VALUE(isMidPixOffset)
  COMPARE_COMPARABLE(midPixOffset)
  return 0;
}

//------------------------------------------------------------------------------

ImageCut::ImageCut() : ImageCut(0, 0, 0, 0) {
}

ImageCut::ImageCut(uint left_, uint top_, uint right_, uint bottom_)
: left(left_), top(top_), right(right_), bottom(bottom_) {
}

int ImageCut::compare(rc that) const {
  COMPARE_VALUE(left)
  COMPARE_VALUE(top)
  COMPARE_VALUE(right)
  COMPARE_VALUE(bottom)
  return 0;
}

size2d ImageCut::marginSize() const {
  return size2d(left + right, top + bottom);
}

//------------------------------------------------------------------------------

Angles::Angles() : Angles(0, 0) {
}

Angles::Angles(gma_t gma_, tth_t tth_) : gma(gma_), tth(tth_) {
}

AngleMap::Key::Key(Geometry::rc geometry_, size2d::rc size_,
                   ImageCut::rc cut_, IJ::rc midPix_, tth_t midTth_)
: geometry(geometry_), size(size_), cut(cut_), midPix(midPix_), midTth(midTth_) {
}

int AngleMap::Key::compare(rc that) const {
  COMPARE_COMPARABLE(geometry)
  COMPARE_COMPARABLE(size)
  COMPARE_COMPARABLE(cut)
  COMPARE_COMPARABLE(midPix)
  COMPARE_VALUE(midTth)
  return 0;
}

//------------------------------------------------------------------------------

AngleMap::AngleMap(Key::rc key) {
  calculate(key);
}

void AngleMap::calculate(Key::rc key) {
  arrAngles_.fill(key.size);
  rgeGma_.invalidate();
  rgeTth_.invalidate();

  if (key.size.isEmpty())
    return;

  qreal pixSize = key.geometry.pixSize,
        detDist = key.geometry.detectorDistance;

  for_int (i, key.size.w) {
    qreal x       = (to_i(i) - key.midPix.i) * pixSize;
    rad   tthHorz = key.midTth.toRad() + atan(x / detDist);
    qreal h       = cos(tthHorz) * hypot(x, detDist);

    for_int (j, key.size.h) {
      qreal y          = (key.midPix.j - to_i(j)) * pixSize;
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
      rgeGma_.extendBy(as.gma);
      rgeTth_.extendBy(as.tth);
    }
  }
}

//------------------------------------------------------------------------------
}
// eof
