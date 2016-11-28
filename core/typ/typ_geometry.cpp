// ************************************************************************** //
//
// STeCa2 - StressTextureCalculator ver. 2
//
// Copyright (C) 2016 Forschungszentrum Jülich GmbH 2016
//
// This program is free software: you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the Free
// Software Foundation, either version 3 of the License, or (at your option)
// any later version.
//
// This program is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
// or FITNESS FOR A PARTICULAR PURPOSE.
//
// See the COPYING and AUTHORS files for more details.
// ************************************************************************** //

#include "typ_geometry.h"
#include "def/def_compare.h"
#include <qmath.h>

namespace typ {
//------------------------------------------------------------------------------

preal const Geometry::MIN_DETECTOR_DISTANCE   = preal(10);
preal const Geometry::MIN_DETECTOR_PIXEL_SIZE = preal(.1);

preal const Geometry::DEF_DETECTOR_DISTANCE   = preal(1035);
preal const Geometry::DEF_DETECTOR_PIXEL_SIZE = preal(1);

Geometry::Geometry()
: detectorDistance(MIN_DETECTOR_DISTANCE), pixSize(MIN_DETECTOR_PIXEL_SIZE)
, midPixOffset() {
}

int Geometry::compare(rc that) const {
  COMPARE_VALUE(detectorDistance)
  COMPARE_VALUE(pixSize)
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

Angles::Angles(tth_t tth_, gma_t gma_) : tth(tth_), gma(gma_) {
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

AngleMap::AngleMap(Key::rc key) : key_(key) {
  calculate();
}

//TODO remove
//IJ AngleMap::gmaPixel(gma_t gma) {
//  uint_vec const *indexes; uint minIndex, maxIndex;
//  getGmaIndexes(Range(gma), indexes, minIndex, maxIndex);

//  auto& geometry = key_.geometry;
//  auto& size     = key_.size;
//  auto& midPix   = key_.midPix;
//  auto& midTth   = key_.midTth;

//  qreal pixSize = geometry.pixSize,
//        detDist = geometry.detectorDistance;

//  qreal d_midTth   = midTth.toRad(),
//        cos_midTth = cos(d_midTth), sin_midTth = sin(d_midTth);

//  auto j = [&](int i) {
//    return midPix.j - tan(gma) * (detDist * sin_midTth + ((i - midPix.i) * pixSize) * cos_midTth) / pixSize;
//  };

////  auto i = [&](int j) {
////    return (((midPix.j - j) * pixSize) / tan(gma) - b_x1) / (pixSize * cos_midTth) + midPix.i;
////  };

//  TR("gma 0" << size.w-1 << ":" << gma << j(0) << j(to_i(size.w-1)))

//  return IJ(0,0);
//}

static uint lowerBound(vec<gma_t>::rc vec, gma_t x, uint i1, uint i2) {
  EXPECT(i1 < i2)

  if (1 == i2-i1)
    return i1;

  uint mid = (i1 + i2) / 2;
  return vec.at(mid-1) < x            // x may be NaN ...
      ? lowerBound(vec, x, mid, i2)
      : lowerBound(vec, x, i1, mid);  // ... we should be so lucky
}

static uint upperBound(vec<gma_t>::rc vec, gma_t x, uint i1, uint i2) {
  EXPECT(i1 < i2)

  if (1 == i2-i1)
    return i2;

  uint mid = (i1 + i2) / 2;
  return vec.at(mid) > x              // x may be NaN ...
      ? upperBound(vec, x, i1, mid)
      : upperBound(vec, x, mid, i2);  // ... we should be so lucky
}

void AngleMap::getGmaIndexes(gma_rge::rc rgeGma,
                             uint_vec const*& indexes, uint& minIndex, uint& maxIndex) const {
  indexes = &gmaIndexes;
  minIndex = lowerBound(gmas, rgeGma.min, 0, gmas.count());
  maxIndex = upperBound(gmas, rgeGma.max, 0, gmas.count());
}

void AngleMap::calculate() {
  auto& geometry = key_.geometry;
  auto& size     = key_.size;
  auto& cut      = key_.cut;
  auto& midPix   = key_.midPix;
  auto& midTth   = key_.midTth;

  qreal pixSize = geometry.pixSize,
        detDist = geometry.detectorDistance;

  arrAngles_.fill(size);

  rgeTth_.invalidate();
  rgeGma_.invalidate();
  rgeGmaFull_.invalidate();

  EXPECT(size.w > cut.left + cut.right)
  EXPECT(size.h > cut.top  + cut.bottom)

  uint countWithoutCut = (size.w - cut.left - cut.right)
                       * (size.h - cut.top  - cut.bottom);
  EXPECT(countWithoutCut > 0)

  gmas.resize(countWithoutCut);
  gmaIndexes.resize(countWithoutCut);

// was: adapted from Steca original code
//  for_int (i, size.w) {
//    qreal x       = (to_i(i) - midPix.i) * pixSize;
//    rad   tthHorz = midTth.toRad() + atan(x / detDist);
//    qreal h       = cos(tthHorz)   * hypot(x, detDist);

//    for_int (j, size.h) {
//      qreal y          = (midPix.j - to_i(j)) * pixSize;
//      qreal z          = hypot(x, y);
//      qreal pixDetDist = hypot(z, detDist);
//      rad   tth        = acos(h / pixDetDist);

//      qreal r     = sqrt((pixDetDist * pixDetDist) - (h * h));
//      rad   gamma = asin(y / r);

//      if (tthHorz < 0) {
//        tth   = -tth;
//        gamma = -gamma;
//      }

//      ENSURE(!qIsNaN(gamma))

//      arrAngles_.setAt(i, j, Angles(tth.toDeg(), gamma.toDeg()));
//    }
//  }

  // new code
  // detector coordinates: d_x, ... (d_z = const)
  // beam coordinates: b_x, ..; b_y = d_y

  qreal d_midTth   = midTth.toRad(),
        cos_midTth = cos(d_midTth), sin_midTth = sin(d_midTth);

  qreal& d_z = detDist;
  qreal b_x1 = d_z * sin_midTth;
  qreal b_z1 = d_z * cos_midTth;

  for_int (i, size.w) {
    qreal d_x = (to_i(i) - midPix.i) * pixSize;

    qreal b_x = b_x1 + d_x * cos_midTth;
    qreal b_z = b_z1 - d_x * sin_midTth;

    qreal b_x2 = b_x*b_x;

    for_int (j, size.h) {
      qreal b_y = (midPix.j - to_i(j)) * pixSize; // == d_y
      qreal b_r = sqrt(b_x2 + b_y*b_y);

      rad   gma = atan2(b_y, b_x);
      rad   tth = atan2(b_r, b_z);

      arrAngles_.setAt(i, j, Angles(tth.toDeg(), gma.toDeg()));
    }
  }

  uint gi = 0;

  for (uint i = cut.left, iEnd = size.w - cut.right; i < iEnd; ++i) {
    for (uint j = cut.top, jEnd = size.h - cut.bottom; j < jEnd; ++j) {
      auto& as = arrAngles_.at(i, j);

      gmas[gi]       = as.gma;
      gmaIndexes[gi] = i + j * size.w;
      ++gi;

      rgeTth_.extendBy(as.tth);
      rgeGmaFull_.extendBy(as.gma);
      if (as.tth >= midTth)
        rgeGma_.extendBy(as.gma); // gma range at mid tth
    }
  }

  uint_vec is(countWithoutCut);
  for_i (is.count())
    is[i] = i;

  std::sort(is.begin(), is.end(), [this](uint i1,uint i2) {
    qreal gma1 = gmas.at(i1), gma2 = gmas.at(i2);
    return gma1 < gma2;
  });

  vec<gma_t> gv(countWithoutCut);
  for_i (countWithoutCut)
    gv[i] = gmas.at(is.at(i));
  gmas = gv;

  uint_vec uv(countWithoutCut);
  for_i (countWithoutCut)
    uv[i] = gmaIndexes.at(is.at(i));
  gmaIndexes = uv;
}

//------------------------------------------------------------------------------
}
// eof
