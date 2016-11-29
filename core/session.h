/*******************************************************************************
 * STeCa2 - StressTextureCalculator ver. 2
 *
 * Copyright (C) 2016 Forschungszentrum Jülich GmbH 2016
 *
 * This program is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the Free
 * Software Foundation, either version 3 of the License, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE.
 * 
 * See the COPYING and AUTHORS files for more details.
 ******************************************************************************/

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

private:
  // correction file
  data::shp_File corrFile_;
  typ::Image     corrImage_;
  bool           corrEnabled_;

  mutable typ::Image intensCorr_;
  mutable bool       corrHasNaNs_;

  void calcIntensCorr() const;

  // datasets
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

  typ::Image const* intensCorr() const;

  void setCorrFile(data::shp_File) THROWS;  // Load or remove a correction file.
  void remCorrFile();

  void tryEnableCorr(bool);

  bool isCorrEnabled() const {
    return corrEnabled_;
  }

  void collectDatasetsFromFiles(uint_vec, pint);

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
  // All files must have images of the same size
  typ::size2d imageSize_;
  // Clears the image size if there are no files in the session.
  void updateImageSize();
  // Ensures that all images have the same size.
  void setImageSize(typ::size2d::rc) THROWS;

public:
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
  typ::Range    gammaRange_;
  mutable typ::cache_lazy<typ::AngleMap::Key,typ::AngleMap> angleMapCache_;

public:
  typ::Geometry::rc geometry() const { return geometry_; }
  void setGeometry(preal detectorDistance, preal pixSize, typ::IJ::rc midPixOffset);
  typ::IJ midPix() const;

  typ::Range::rc gammaRange() const;
  void setGammaRange(typ::Range::rc);

  typ::shp_AngleMap        angleMap(data::OneDataset::rc) const;
  static typ::shp_AngleMap angleMap(Session::rc, data::OneDataset::rc);

// lenses
public:
  calc::shp_ImageLens   imageLens(typ::Image::rc, data::Datasets::rc,
                                  bool trans, bool cut) const;
  calc::shp_DatasetLens datasetLens(data::Dataset::rc, data::Datasets::rc, eNorm,
                                    bool trans, bool cut) const;
  typ::Curve makeCurve(calc::DatasetLens::rc, gma_rge::rc, bool averaged) const;

  // reflections
  calc::ReflectionInfo makeReflectionInfo(
      calc::DatasetLens::rc, calc::Reflection::rc, gma_rge::rc, bool averaged) const;

  calc::ReflectionInfos makeReflectionInfos(
      data::Datasets::rc, calc::Reflection::rc,
      uint gmaSlices, gma_rge::rc, bool averaged, Progress*);

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

// normalisation
private:
  eNorm norm_;

public:
  eNorm norm() const { return norm_; }
  void  setNorm(eNorm);

public:
  qreal calcAvgBackground(data::Dataset::rc) const;
  qreal calcAvgBackground(data::Datasets::rc) const;
};

//------------------------------------------------------------------------------
}
#endif
