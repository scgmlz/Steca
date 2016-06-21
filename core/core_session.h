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

#include "core_file.h"
#include "types/core_type_geometry.h"
#include "types/core_type_image.h"
#include "types/core_type_image_transform.h"
#include "types/core_types_fwd.h"

class Progress;

namespace core {
//------------------------------------------------------------------------------

class Session final {
public:
  Session();

  void clear();

  // data files
private:
  vec<shp_File> files_;

public:
  /// number of data files (not counting the correction file)
  uint     numFiles() const { return files_.count(); }
  shp_File file(uint i) const;

  bool hasFile(rcstr fileName);
  void addFile(shp_File) THROWS;
  void remFile(uint i);

  // correction file
private:
  shp_File corrFile_;
  Image    corrImage_;
  bool     corrEnabled_;

  uint_vec collectedFromFiles_;  ///< from these files
  Datasets collectedDatasets_;   ///< datasets collected ...
  str_lst  collectedDatasetsTags_;

public:
  bool     hasCorrFile() const { return !corrFile_.isNull(); }
  shp_File corrFile() const { return corrFile_; }
  rcImage  corrImage() const { return corrImage_; }

  void setCorrFile(shp_File) THROWS;  ///< Load or remove a correction file.
  void remCorrFile();

  void tryEnableCorr(bool);
  bool isCorrEnabled() const { return corrEnabled_; }

  void collectDatasetsFromFiles(uint_vec, uint);

  uint_vec const& collectedFromFiles() const { return collectedFromFiles_; }
  rcDatasets      collectedDatasets()  const { return collectedDatasets_;  }

  str_lst const&  collectedDatasetsTags() const {
    return collectedDatasetsTags_;
  }

private:
  /// All files must have images of the same size; this is a cached value
  size2d imageSize_;
  /// Clears the image size if there are no files in the session.
  void updateImageSize();
  /// Ensures that all images have the same size.
  void setImageSize(size2d const&) THROWS;

  size2d imageSize() const;

  // image - transform & cut etc.
private:
  ImageTransform imageTransform_;
  ImageCut       imageCut_;

public:
  ImageTransform const& imageTransform() const { return imageTransform_; }
  ImageCut const&       imageCut()       const { return imageCut_;       }

  void setImageTransformMirror(bool);
  void setImageTransformRotate(ImageTransform const&);

  void setImageCut(bool topLeftFirst, bool linked, ImageCut const&);

  AngleMap const& angleMap(rcDataset) const;

private:
  Geometry geometry_;

public:
  Geometry const& geometry() const { return geometry_; }
  void setGeometry(qreal detectorDistance, qreal pixSize, bool isMidPixOffset,
                   rcIJ midPixOffset);
  IJ midPix() const;

// lenses
public:
  shp_ImageLens lens(rcImage, rcDatasets, bool trans, bool cut) const;
  shp_Lens lens(rcDataset, rcDatasets, bool trans, bool cut, eNorm) const;

  Curve makeCurve(shp_Lens, rcRange gammaSector) const;

  // reflections
  ReflectionInfo makeReflectionInfo(shp_Lens, rcReflection,
                                    rcRange gammaSector) const;

  ReflectionInfos makeReflectionInfos(rcDatasets, rcReflection, deg betaStep,
                                      rcRange gammaRange, Progress* = nullptr);
// fitting
private:
  uint   bgPolyDegree_;
  Ranges bgRanges_;

  Reflections reflections_;

public:
  rcRanges      bgRanges()      const { return bgRanges_; }
  uint          bgPolyDegree()  const { return bgPolyDegree_; }
  rcReflections reflections()   const { return reflections_; }

  void setBgRanges(rcRanges);
  bool addBgRange(rcRange);
  bool remBgRange(rcRange);

  void setBgPolyDegree(uint);

  void addReflection(shp_Reflection);
  void remReflection(uint);

// normalization
private:
  eNorm norm_;

public:
  eNorm norm() const { return norm_; }
  void  setNorm(eNorm);

public:
  qreal calcAvgBackground(rcDataset) const;
  qreal calcAvgBackground(rcDatasets) const;
};

//------------------------------------------------------------------------------
}
#endif
