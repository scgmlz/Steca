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
  uint numFiles(bool withCorr=false);

  void addFile(rcstr fileName) THROWS;  ///< Add an ordinary file to the session.
  bool hasFile(rcstr fileName);         ///< Is there this ordinary file?

  shp_File const& getFile(uint i);      ///< Access the i-th file, including the correction file.
  void remFile(uint i);                 ///< Remove the i-th file, including the correction file.

  void loadCorrFile(rcstr fileName);    ///< Load a correction file.
  bool hasCorrFile() const;

protected:
  QVector<shp_File> dataFiles;

private:
  QSize imageSize; ///< All files must have images of the same size; this is a cached value

  void setImageSize(QSize const&) THROWS; ///< Ensures that all images have the same size.
  void updateImageSize();                 ///< Clears the image size if there are no files in the session.

public: // detector TODO make a structure; rename variables
  qreal pixSpan;            // size of the detector pixel
  qreal sampleDetectorSpan; // the distance between sample - detector // TODO verify: in adhoc has at least three names: sampleDetectorSpan, detectorSampleSpan, detectorSampleDistance
  bool  hasBeamOffset;
  int   middlePixXOffset;
  int   middlePixYOffset;

public: // image transform
  Image::Transform imageTransform;

  void setImageMirror(bool);
  void setImageRotate(core::Image::Transform);

protected: // corrections
  struct Pixpos {  // TODO bad names
    Pixpos(): Pixpos(0,0) {}
    Pixpos(qreal gamma, qreal tth): gammaPix(gamma), tthPix(tth) {}
    qreal gammaPix;
    qreal tthPix;
  };

  // TODO rename;
  typedef core::Array2D<Pixpos> AngleCorrArray;
  AngleCorrArray angleCorrArray;
  Borders        ful, cut;

  QPoint  getPixMiddle(QSize const& imageSize) const;  // TODO rename, was getPixMiddleX/Y

  // TODO cashing of calcAngle...
  qreal lastCalcTthMitte; QPoint lastPixMiddle;
  qreal lastPixSpan, lastSampleDetectorSpan;
  ImageCut lastImageCut;

  shp_File corrFile;

public:
  AngleCorrArray const& calcAngleCorrArray(qreal tthMitte);  // TODO rename; TODO if too slow, cache

public: // TODO not public
  Image intensCorrArray;  // summed corrFile intensities
  void calcIntensCorrArray();

  Borders const& getCut() const { return cut; }

public: // image cut
  ImageCut const& getImageCut() const { return imageCut; }
  void setImageCut(bool topLeft, bool linked, ImageCut const&);

protected:
  ImageCut imageCut;
};

}

#endif
