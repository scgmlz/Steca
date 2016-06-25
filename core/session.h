// ************************************************************************** //
//
//  STeCa2:    StressTextureCalculator ver. 2
//
//! @file      core_session.h
//! @brief     Session that can compute all and needs no GUI.
//!
//! @homepage  http://apps.jcns.fz-juelich.de/steca2
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016
//! @authors   Scientific Computing Group at MLZ Garching
//! @authors   Rebecca Brydon, Jan Burle,  Antti Soininen
//! @authors   Based on the original STeCa by Christian Randau
//
// ************************************************************************** //

#ifndef CORE_SESSION_H
#define CORE_SESSION_H

#include "calc/calc_lens.h"
#include "calc/calc_reflection.h"
#include "calc/calc_reflection_info.h"
#include "data/data_file.h"
#include "typ/typ_async.h"
#include "typ/typ_cache.h"
#include "typ/typ_geometry.h"
#include "typ/typ_image.h"
#include "typ/typ_image_transform.h"

namespace core {
//------------------------------------------------------------------------------

class Session final {
  CLS(Session)
public:
  Session();

  void clear();

  // data files
private:
  typ::vec<data::shp_File> files_;

public:
  // number of data files (not counting the correction file)
  uint           numFiles()   const { return files_.count(); }
  data::shp_File file(uint i) const;

  bool hasFile(rcstr fileName);
  void addFile(data::shp_File) THROWS;
  void remFile(uint i);

  // correction file
private:
  data::shp_File corrFile_;
  typ::Image     corrImage_;
  bool           corrEnabled_;

  uint_vec       collectedFromFiles_;  // from these files
  data::Datasets collectedDatasets_;   // datasets collected ...
  str_lst        collectedDatasetsTags_;

public:
  bool hasCorrFile() const {
    return !corrFile_.isNull();
  }

  data::shp_File corrFile() const {
    return corrFile_;
  }

  typ::Image::rc corrImage() const {
    return corrImage_;
  }

  void setCorrFile(data::shp_File) THROWS;  // Load or remove a correction file.
  void remCorrFile();

  void tryEnableCorr(bool);

  bool isCorrEnabled() const {
    return corrEnabled_;
  }

  void collectDatasetsFromFiles(uint_vec, nint);

  uint_vec::rc collectedFromFiles() const {
    return collectedFromFiles_; \
  }

  data::Datasets::rc collectedDatasets() const {
    return collectedDatasets_;
  }

  str_lst::rc  collectedDatasetsTags() const {
    return collectedDatasetsTags_;
  }

private:
  // All files must have images of the same size; this is a cached value
  typ::size2d imageSize_;
  // Clears the image size if there are no files in the session.
  void updateImageSize();
  // Ensures that all images have the same size.
  void setImageSize(typ::size2d::rc) THROWS;

  typ::size2d imageSize() const;

  // image - transform & cut etc.
private:
  typ::ImageTransform imageTransform_;
  typ::ImageCut       imageCut_;

public:
  typ::ImageTransform::rc imageTransform() const { return imageTransform_; }
  typ::ImageCut::rc       imageCut()       const { return imageCut_;       }

  void setImageTransformMirror(bool);
  void setImageTransformRotate(typ::ImageTransform::rc);

  void setImageCut(bool topLeftFirst, bool linked, typ::ImageCut::rc);

private:
  typ::Geometry geometry_;
  mutable typ::cache_lazy<typ::AngleMap::Key,typ::AngleMap> angleMapCache_;

public:
  typ::Geometry::rc geometry() const { return geometry_; }
  void setGeometry(qreal detectorDistance, qreal pixSize, bool isMidPixOffset,
                   typ::IJ::rc midPixOffset);
  typ::IJ midPix() const;

  typ::shp_AngleMap        angleMap(data::OneDataset::rc) const;
  static typ::shp_AngleMap angleMap(Session::rc, data::OneDataset::rc);

// lenses
public:
  calc::shp_ImageLens imageLens(typ::Image::rc, data::Datasets::rc,
                                bool trans, bool cut) const;
  calc::shp_Lens      lens(data::Dataset::rc, data::Datasets::rc, eNorm,
                           bool trans, bool cut) const;

  typ::Curve makeCurve(calc::shp_Lens, gma_rge::rc) const;

  // reflections
  calc::ReflectionInfo makeReflectionInfo(
      calc::shp_Lens, calc::Reflection::rc, gma_rge::rc) const;

  calc::ReflectionInfos makeReflectionInfos(
      data::Datasets::rc, calc::Reflection::rc,
      gma_t gmaStep, gma_rge::rc, Progress* = nullptr);
// fitting
private:
  uint   bgPolyDegree_;
  typ::Ranges bgRanges_;

  calc::Reflections reflections_;

public:
  typ::Ranges::rc       bgRanges()      const { return bgRanges_; }
  uint                  bgPolyDegree()  const { return bgPolyDegree_; }
  calc::Reflections::rc reflections()   const { return reflections_; }

  void setBgRanges(typ::Ranges::rc);
  bool addBgRange(typ::Range::rc);
  bool remBgRange(typ::Range::rc);

  void setBgPolyDegree(uint);

  void addReflection(calc::shp_Reflection);
  void remReflection(uint);

// normalization
private:
  eNorm norm_;

public:
  eNorm norm() const { return norm_; }
  void setNorm(eNorm);

public:
  qreal calcAvgBackground(data::Dataset::rc) const;
  qreal calcAvgBackground(data::Datasets::rc) const;
};

//------------------------------------------------------------------------------
}
#endif
