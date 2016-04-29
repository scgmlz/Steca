// ************************************************************************** //
//
//  STeCa2:    StressTexCalculator ver. 2
//
//! @file      core_session.h
//! @brief     Session that can compute all and needs no GUI.
//!
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016
//! @authors   Scientific Computing Group at MLZ Garching
//! @authors   Original version: Christian Randau
//! @authors   Version 2: Antti Soininen, Jan Burle, Rebecca Brydon
//
// ************************************************************************** //

#ifndef CORE_SESSION_H
#define CORE_SESSION_H

#include "core_file.h"
#include "types/core_type_image_transform.h"
#include "types/core_type_geometry.h"
#include "types/core_types_fwd.h"
#include <QStringList>

namespace core {
//------------------------------------------------------------------------------

class Session final { TESTS_FRIEND
public:
  Session();
  void clear();

// data files
private:
  QVector<shp_File> files_;

public:
  /// number of data files (not counting the correction file)
  uint     numFiles()       const { return files_.count(); }
  shp_File file(uint i)     const;

  bool     hasFile(rcstr fileName);
  void     addFile(shp_File) THROWS;
  void     remFile(uint i);

// correction file
private:
  shp_File corrFile_;
  Image    corrImage_;
  bool     corrEnabled_;

  uint_vec collectedFromFiles_;
  Datasets collectedDatasets_;
  str_lst  combindedDatasetsIndices_;

public:
  bool     hasCorrFile()    const { return !corrFile_.isNull(); }
  shp_File corrFile()       const { return corrFile_;           }
  rcImage  corrImage()      const { return corrImage_;          }

  void     setCorrFile(shp_File) THROWS;    ///< Load or remove a correction file.
  void     remCorrFile();

  void     enableCorr(bool);
  bool     isCorrEnabled()  const { return corrEnabled_;        }

  void            collectDatasetsFromFiles(uint_vec);
  uint_vec const& collectedFromFiles() { return collectedFromFiles_;  }
  rcDatasets      collectedDatasets()  const;
  uint&           numCombinedDatasets();
  str_lst  const& combinedDatasetsIndices();

private:
  uint            numCombine_;
// image - sanity
private:
  /// All files must have images of the same size; this is a cached value
  QSize imageSize_;
  /// Clears the image size if there are no files in the session.
  void updateImageSize();
  /// Ensures that all images have the same size.
  void setImageSize(QSize const&) THROWS;

  QSize imageSize() const;

// image - transform & cut etc.
private:
  ImageTransform imageTransform_;
  ImageCut       imageCut_;

public:
  ImageTransform const& imageTransform() const { return imageTransform_; }
  ImageCut       const& imageCut()       const { return imageCut_;       }

  void setImageTransformMirror(bool);
  void setImageTransformRotate(ImageTransform const&);

  void setImageCut(bool topLeftFirst, bool linked, ImageCut const&);

  AngleMap const& angleMap(rcDataset) const;

// geometry
private:
  Geometry geometry_;

public:
  Geometry const& geometry() const { return geometry_; }
  void setGeometry(qreal detectorDistance, qreal pixSize,
                   bool isMidPixOffset, rcIJ midPixOffset);
  IJ midPix() const;

// lenses
public:
  shp_ImageLens lens(rcImage,   rcDatasets, bool trans, bool cut) const;
  shp_Lens      lens(rcDataset, rcDatasets, bool trans, bool cut, eNorm) const;

// reflections
  ReflectionInfo makeReflectionInfo(shp_Lens, rcReflection,
                                    rcRange gammaSector) const;

  ReflectionInfos reflectionInfos(rcDatasets, rcReflection, qreal betaStep,
                                  rcRange gammaRange = Range());
// fitting
private:
  uint   bgPolynomialDegree_;
  Ranges bgRanges_;

  Reflections reflections_;

public:
  // TODO instead of exposing the objects, provide an interface for TheHub
  Ranges&       bgRanges()           { return bgRanges_;           }
  uint&         bgPolynomialDegree() { return bgPolynomialDegree_; }
  Reflections&  reflections()        { return reflections_;        }

// normalization
private:
  eNorm norm_;

public:
  eNorm norm() const { return norm_; }
  void setNorm(eNorm);

public:
  qreal calcAvgBackground(rcDataset)  const;
  qreal calcAvgBackground(rcDatasets) const;
};

//------------------------------------------------------------------------------
}
#endif
