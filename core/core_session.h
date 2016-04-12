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
#include "core_fit_functions.h"
#include "core_lens.h"
#include "core_reflection.h"
#include "types/core_type_image_transform.h"
#include "types/core_type_geometry.h"

namespace core {
//------------------------------------------------------------------------------

class Session final {
public:
  Session();
  void clear();

// data files
private:
  QVector<shp_File> _files;

public:
  /// how many files has, optionally also counting the correction file.
  uint     numFiles(bool withCorr) const;
  shp_File file(uint i) const;              ///< Access the i-th file, including the correction file.

  void     addFile(shp_File) THROWS;        ///< Add an ordinary file to the session.
  void     remFile(uint i);                 ///< Remove the i-th file, NOT including the correction file.
  bool     hasFile(rcstr fileName);         ///< Is there this ordinary file?

// correction file
private:
  shp_File _corrFile;
  bool     _corrEnabled;

public:
  shp_File corrFile()       const   { return _corrFile;           }
  bool     hasCorrFile()    const   { return !_corrFile.isNull(); }

  void     setCorrFile(shp_File) THROWS;    ///< Load or remove a correction file.
  void     remCorrFile();

  void     enableCorr(bool);
  bool     isCorrEnabled()  const   { return _corrEnabled;        }

// image - sanity
private:
  /// All files must have images of the same size; this is a cached value
  QSize _imageSize;
  /// Clears the image size if there are no files in the session.
  void updateImageSize();
  /// Ensures that all images have the same size.
  void setImageSize(QSize const&) THROWS;

// image - transform & cut etc.
private:
  ImageTransform _imageTransform;
  ImageCut       _imageCut;

public:
  ImageTransform const& imageTransform() const { return _imageTransform; }
  ImageCut       const& imageCut()       const { return _imageCut;       }

  void setImageTransformMirror(bool);
  void setImageTransformRotate(ImageTransform const&);

  void setImageCut(bool topLeftFirst, bool linked, ImageCut const&);

  AngleMap const& angleMap(rcDataset) const;

// geometry
private:
  Geometry _geometry;

public:
  Geometry const& geometry() const { return _geometry; }
  void setGeometry(qreal detectorDistance, qreal pixSize,
                   bool isMidPixOffset, rcIJ midPixOffset);
  IJ midPix() const;

// lenses
public:
  shp_Lens lens(bool trans, bool cut, Lens::eNorm, rcDataset) const;

// fitting
private:
  uint   _bgPolynomialDegree;
  Ranges _bgRanges;

  Reflections _reflections;

public:
  // TODO instead of exposing the objects, provide an interface for TheHub
  core::Ranges&       bgRanges()           { return _bgRanges;           }
  uint&               bgPolynomialDegree() { return _bgPolynomialDegree; }
  core::Reflections&  reflections()        { return _reflections;        }

// normalization
private:
  Lens::eNorm _norm;

public:
  Lens::eNorm norm() const { return _norm; }
  void setNorm(Lens::eNorm);

public:
  qreal calcAvgBackground(rcDataset)  const;
  qreal calcAvgBackground(rcDatasets) const;
};

//------------------------------------------------------------------------------
}
#endif
