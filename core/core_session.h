/** \file
 * Session that can compute all and needs no GUI.
 */

#ifndef CORE_SESSION_H
#define CORE_SESSION_H

#include "core_lib.h"
#include "core_file.h"
#include "core_array2d.h"
#include <QPoint>

namespace core {

class Session {
public:
  Session();
  virtual ~Session();

  /// How many files has, optionally also counting the correction file.
  uint     numFiles(bool withCorr=false) const;

  shp_File addFile(rcstr fileName) THROWS;  ///< Add an ordinary file to the session.
  shp_File remFile(uint i);                 ///< Remove the i-th file, NOT including the correction file.
  bool     hasFile(rcstr fileName);         ///< Is there this ordinary file?

  shp_File getFile(uint i) const;           ///< Access the i-th file, including the correction file.

  shp_File loadCorrFile(rcstr fileName);    ///< Load or remove a correction file.
  void     remCorrFile();
  shp_File getCorrFile() const;
  bool     hasCorrFile() const { return !getCorrFile().isNull(); }

private:
  QVector<shp_File> dataFiles;
  shp_File corrFile;

private:
  QSize imageSize; ///< All files must have images of the same size; this is a cached value

  void setImageSize(QSize const&) THROWS; ///< Ensures that all images have the same size.
  void updateImageSize();                 ///< Clears the image size if there are no files in the session.

public: // detector geometry
  struct Geometry {
    Geometry();
    // TODO rename "span" -> ...
    qreal sampleDetectorSpan; // the distance between sample - detector // TODO verify: in adhoc has at least three names: sampleDetectorSpan, detectorSampleSpan, detectorSampleDistance
    qreal pixSpan;            // size of the detector pixel
    bool  hasBeamOffset;
    QPoint middlePixOffset;
  };

  Geometry const& getGeometry() const { return geometry; }
  void setGeometry(qreal sampleDetectorSpan, qreal pixSpan, bool hasBeamOffset, QPoint const& middlePixOffset);

private:
  Geometry geometry;

public: // image transform
  ImageTransform imageTransform;

  void setImageMirror(bool);
  void setImageRotate(core::ImageTransform);

protected: // corrections TODO make private
  // TODO rename;
  typedef core::Array2D<Pixpos> AngleCorrArray;
  AngleCorrArray angleCorrArray;
  Borders        ful, cut;

  QPoint  getPixMiddle(QSize const& imageSize) const;  // TODO rename, was getPixMiddleX/Y

  // TODO cashing of calcAngle...
  qreal lastCalcTthMitte; QPoint lastPixMiddle;
  qreal lastPixSpan, lastSampleDetectorSpan;
  ImageCut lastImageCut;

public:
  AngleCorrArray const& calcAngleCorrArray(qreal tthMitte);  // TODO rename; TODO if too slow, cache

public: // TODO not public
  Image intensCorrArray;  // summed corrFile intensities
  bool  hasNaNs;
  void  calcIntensCorrArray();

  Borders const& getCut() const { return cut; }

public: // image cut
  ImageCut const& getImageCut() const { return imageCut; }
  void setImageCut(bool topLeft, bool linked, ImageCut const&);

private:
  ImageCut imageCut;
};

}

#endif
