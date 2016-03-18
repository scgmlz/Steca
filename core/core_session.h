/** \file
 * Session that can compute all and needs no GUI.
 */

#ifndef CORE_SESSION_H
#define CORE_SESSION_H

#include "core_angle_map_array.h"
#include "core_file.h"
#include "core_fit_functions.h"
#include "core_lens.h"
#include "core_reflection.h"
#include "core_image_cut.h"
#include "core_image_transform.h"
#include <QPoint>

namespace core {
//------------------------------------------------------------------------------

/// detector geometry
struct Geometry {
  static qreal const MIN_DETECTOR_DISTANCE;
  static qreal const MIN_DETECTOR_PIXEL_SIZE;

  Geometry();
  bool operator ==(Geometry const&) const;

  qreal detectorDistance;   // the distance from the sample to the detector
  qreal pixSize;            // size of the detector pixel
  bool  hasBeamOffset;
  QPoint middlePixOffset;
};

struct Borders { // REVIEW bad name, hide, remove?
  Range
    gamma,
    tth_regular,
    tth_gamma0; // at gamma=0

  void invalidate() {
    gamma.invalidate();
    tth_regular.invalidate();
    tth_gamma0.invalidate();
  }

  bool isValid() const {
    return gamma.isValid() && tth_regular.isValid() && tth_gamma0.isValid();
  }
};

class Session final {
public:
  Session();
  virtual ~Session();

  void clear();

  /// How many files has, optionally also counting the correction file.
  uint     numFiles(bool withCorr) const;

  shp_File addFile(rcstr fileName) THROWS;  ///< Add an ordinary file to the session.
  shp_File remFile(uint i);                 ///< Remove the i-th file, NOT including the correction file.
  bool     hasFile(rcstr fileName);         ///< Is there this ordinary file?

  shp_File getFile(uint i) const;           ///< Access the i-th file, including the correction file.

  shp_File loadCorrFile(rcstr fileName);    ///< Load or remove a correction file.
  void     remCorrFile();
  shp_File getCorrFile()   const;
  bool     hasCorrFile()   const  { return !getCorrFile().isNull(); }
  void     enableCorrection(bool);
  bool     isCorrEnabled() const  { return corrEnabled;             }

private:
  QVector<shp_File> dataFiles;
  shp_File corrFile;
  bool     corrEnabled;

private:
  QSize imageSize; ///< All files must have images of the same size; this is a cached value

  void updateImageSize();                 ///< Clears the image size if there are no files in the session.

private:
  void setImageSize(QSize const&) THROWS; ///< Ensures that all images have the same size.

public:

  Geometry const& getGeometry() const { return geometry; }
  void setGeometry(qreal detectorDistance, qreal pixSize, bool hasBeamOffset, QPoint const& middlePixOffset);

private:
  Geometry       geometry;
  ImageTransform imageTransform;

public:
  void setImageMirror(bool);
  void setImageRotate(ImageTransform);

  ImageTransform getImageTransform() const;

  QSize getImageSize() const;

  shp_LensSystem allLenses(Dataset const& dataset,
                           bool const globalIntensityScale);
  shp_LensSystem noROILenses(Dataset const& dataset,
                             bool const globalIntensityScale);
  shp_LensSystem plainLens(Dataset const& dataset);

private: // corrections
  AngleMapArray angleMapArray;

  Borders        ful, cut;      // REVIEW ful - remove?

  QPoint  getPixMiddle() const;  // REVIEW / RENAME

  qreal lastCalcTthMitte; QPoint lastPixMiddle;
  Geometry lastGeometry;

  ImageCut lastImageCut;
  ImageTransform lastImageTransform;

public:
  AngleMapArray const& calcAngleMap(qreal tthMitte);

private:
  Array2D<qreal> intensCorrArray;  // summed corrFile intensities
  bool  hasNaNs;           // TODO warn on the statusbar

public:
  void  calcIntensCorrArray();

  Borders const& getCut() const { return cut; }

public: // image cut
  ImageCut const& getImageCut() const { return imageCut; }
  void setImageCut(bool topLeft, bool linked, ImageCut const&);

private:
  ImageCut imageCut;

public:
  Ranges&                getBgRanges()           { return bgRanges;     }
  Ranges const&          getBgRanges() const     { return bgRanges;     }
  fit::Polynomial&       getBgPolynomial()       { return bgPolynomial; }
  fit::Polynomial const& getBgPolynomial() const { return bgPolynomial; }
  Reflections&           getReflections()        { return reflections;  }

private:
  Ranges          bgRanges;
  fit::Polynomial bgPolynomial;

  Reflections     reflections;
};

//------------------------------------------------------------------------------
}
#endif
