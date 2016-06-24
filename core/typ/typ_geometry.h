// ************************************************************************** //
//
//  STeCa2:    StressTextureCalculator ver. 2
//
//! @file      typ_geometry.h
//! @brief     Geometry.
//!
//! @homepage  http://apps.jcns.fz-juelich.de/steca2
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016
//! @authors   Scientific Computing Group at MLZ Garching
//! @authors   Rebecca Brydon, Jan Burle,  Antti Soininen
//! @authors   Based on the original STeCa by Christian Randau
//
// ************************************************************************** //

#ifndef TYP_GEOMETRY_H
#define TYP_GEOMETRY_H

#include "def/defs.h"
#include "typ/typ_angles.h"
#include "typ/typ_array2d.h"
#include "typ/typ_ij.h"
#include "typ/typ_range.h"
#include <QSharedPointer>

namespace typ {
//------------------------------------------------------------------------------
// detector geometry

struct Geometry {
  THIS(Geometry)

  static qreal const MIN_DETECTOR_DISTANCE;  // REVIEW
  static qreal const MIN_DETECTOR_PIXEL_SIZE;

  Geometry();
  int compare(rc) const;

  qreal detectorDistance;  // the distance from the sample to the detector
  qreal pixSize;           // size of the detector pixel
  bool  isMidPixOffset;
  IJ    midPixOffset;
};

//------------------------------------------------------------------------------
// image cut (margins)

struct ImageCut {
  THIS(ImageCut)

  uint left, top, right, bottom;

  ImageCut();
  ImageCut(uint left, uint top, uint right, uint bottom);

  int compare(rc) const;

  size2d marginSize() const;
};

//------------------------------------------------------------------------------

struct Angles {
  THIS(Angles)

  deg gamma, tth;

  Angles();
  Angles(deg gamma, deg tth);
};

class AngleMap {
  THIS(AngleMap)
public:
  struct Key {
    THIS(Key)

    Key(Geometry::rc, size2d::rc, ImageCut::rc, IJ::rc midPix, deg midTth);

    int compare(rc) const;

    bool operator<(rc that) const {
      return compare(that) < 0;
    }

    Geometry geometry;
    size2d   size;
    ImageCut cut;
    IJ       midPix;
    deg      midTth;
  };

  AngleMap(Key::rc);

  Angles::rc at(uint i, uint j) const {
    return arrAngles_.at(i, j);
  }

  Range::rc rgeGamma() const { return rgeGamma_; }
  Range::rc rgeTth()   const { return rgeTth_;   }

private:
  void calculate(Key::rc);

  Array2D<Angles> arrAngles_;
  Range           rgeGamma_, rgeTth_;
};

typedef QSharedPointer<AngleMap> shp_AngleMap;

//------------------------------------------------------------------------------
}
#endif // TYP_GEOMETRY_H
