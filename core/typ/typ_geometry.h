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
//! @authors   Rebecca Brydon, Jan Burle, Antti Soininen
//! @authors   Based on the original STeCa by Christian Randau
//
// ************************************************************************** //

#ifndef TYP_GEOMETRY_H
#define TYP_GEOMETRY_H

#include "def/defs.h"
#include "typ/typ_array2d.h"
#include "typ/typ_ij.h"
#include "typ/typ_types.h"
#include <QSharedPointer>

namespace typ {
//------------------------------------------------------------------------------
// detector geometry

struct Geometry {
  CLS(Geometry)

  static preal const MIN_DETECTOR_DISTANCE;
  static preal const MIN_DETECTOR_PIXEL_SIZE;

  static preal const DEF_DETECTOR_DISTANCE;
  static preal const DEF_DETECTOR_PIXEL_SIZE;

  Geometry();
  int compare(rc) const;

  preal detectorDistance;  // the distance from the sample to the detector
  preal pixSize;           // size of the detector pixel
  IJ    midPixOffset;
};

//------------------------------------------------------------------------------
// image cut (margins)

struct ImageCut {
  CLS(ImageCut)

  uint left, top, right, bottom;

  ImageCut();
  ImageCut(uint left, uint top, uint right, uint bottom);

  int compare(rc) const;

  size2d marginSize() const;
};

//------------------------------------------------------------------------------

struct Angles {
  CLS(Angles)

  tth_t tth;
  gma_t gma;

  Angles();
  Angles(tth_t, gma_t);
};

class AngleMap {
  CLS(AngleMap)
public:
  struct Key {
    CLS(Key)

    Key(Geometry::rc, size2d::rc, ImageCut::rc, IJ::rc midPix, tth_t midTth);

    int compare(rc) const;

    bool operator<(rc that) const {
      return compare(that) < 0;
    }

    Geometry geometry;
    size2d   size;
    ImageCut cut;
    IJ       midPix;
    tth_t    midTth;
  };

  AngleMap(Key::rc);

  Angles::rc at(uint i) const {
    return arrAngles_.at(i);
  }

  Angles::rc at(uint i, uint j) const {
    return arrAngles_.at(i, j);
  }

  tth_rge rgeTth() const { return rgeTth_; }
  gma_rge rgeGma() const { return rgeGma_; }

//TODO remove  IJ gmaPixel(gma_t);

  void getGmaIndexes(gma_rge::rc, uint_vec const*&, uint&, uint&) const;

private:
  void calculate();

  Key key_;

  Array2D<Angles> arrAngles_;

  tth_rge    rgeTth_;
  gma_rge    rgeGma_;

  // sorted
  vec<gma_t> gmas;
  uint_vec   gmaIndexes;
};

typedef QSharedPointer<AngleMap> shp_AngleMap;

//------------------------------------------------------------------------------
}
#endif // TYP_GEOMETRY_H
