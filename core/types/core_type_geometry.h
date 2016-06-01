// ************************************************************************** //
//
//  STeCa2:    StressTextureCalculator ver. 2
//
//! @file      core_type_geometry.h
//! @brief     Geometry.
//!
//! @homepage  http://apps.jcns.fz-juelich.de/steca2
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016
//! @authors   Scientific Computing Group at MLZ Garching
//! @authors   Antti Soininen, Jan Burle, Rebecca Brydon
//! @authors   Based on the original STeCa by Christian Randau
//
// ************************************************************************** //

#ifndef CORE_TYPE_GEOMETRY_H
#define CORE_TYPE_GEOMETRY_H

#include "core_defs.h"
#include "types/core_angles.h"
#include "types/core_coords.h"
#include "types/core_type_array2d.h"
#include "types/core_type_range.h"

namespace core {
//------------------------------------------------------------------------------
/// detector geometry

struct Geometry {
  static qreal const MIN_DETECTOR_DISTANCE;  // REVIEW
  static qreal const MIN_DETECTOR_PIXEL_SIZE;

  Geometry();
  bool operator==(Geometry const&) const;

  qreal detectorDistance;  // the distance from the sample to the detector
  qreal pixSize;           // size of the detector pixel
  bool  isMidPixOffset;
  IJ    midPixOffset;
};

//------------------------------------------------------------------------------
/// image cut (margins)

struct ImageCut {
  uint left, top, right, bottom;

  ImageCut();
  ImageCut(uint left, uint top, uint right, uint bottom);
  bool operator==(ImageCut const&) const;

  QSize marginSize() const;
};

//------------------------------------------------------------------------------

struct Angles {
  deg gamma, tth;

  Angles();
  Angles(deg gamma, deg tth);
};

class AngleMap {
public:
  AngleMap();

  Angles const& at(uint i, uint j) const { return arrAngles_.at(i, j); }

  rcRange rgeGamma() const { return rgeGamma_; }
  rcRange rgeTth()   const { return rgeTth_;   }

  void calculate(deg midTth, Geometry const&, QSize const&, ImageCut const& cut,
                 rcIJ midPix);

private:
  Array2D<Angles> arrAngles_;
  Range           rgeGamma_, rgeTth_;
};

//------------------------------------------------------------------------------
}
#endif  // CORE_TYPE_GEOMETRY_H
