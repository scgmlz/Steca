/** \file
 * Session that can compute all and needs no GUI.
 */

#ifndef CORE_SESSION_H
#define CORE_SESSION_H

#include "core_lib.h"
#include "core_file.h"
#include <QPoint>

namespace core {

class Session {
public:
  Session();
  virtual ~Session();

  /// How many files has, optionally also counting the correction file.
  uint numFiles(bool withCorr=false);

  void addFile(rcstr fileName) THROWS;  ///< Add an ordinary file.
  bool hasFile(rcstr fileName);         ///< Is there this ordinary file?

  File const& getFile(uint i);          ///< Access the i-th file, including the correction file.
  void remFile(uint i);                 ///< Remove the i-th file, including the correction file.

  bool hasCorrFile() const;
  void loadCorrFile(rcstr fileName);    ///< Load a correction file.

protected:
  QVector<shp_File> dataFiles;

private:
  uint imageSize; ///< All files have images of the same size; this is a cached value

  void setImageSize(uint) THROWS;       ///< Ensures that all images have the same size.
  void updateImageSize();               ///< Clears image size if there are no more images.

public: // detector TODO make a structure; rename variables
  qreal pixSpan;            // size of the detector pixel
  qreal sampleDetectorSpan; // the distance between sample - detector // TODO verify: in adhoc has at least three names: sampleDetectorSpan, detectorSampleSpan, detectorSampleDistance
  bool  hasBeamOffset;
  int   middlePixXOffset;
  int   middlePixYOffset;

public: // image transform
  void setMirror(bool);
  void setRotate(core::Image::Transform);

public: // image
  struct imagecut_t {
    imagecut_t(uint top = 0, uint bottom = 0, uint left = 0, uint right = 0);
    bool operator==(imagecut_t const&);
    uint top, bottom, left, right;

    uint getWidth(uint imageSize) const;
    uint getHeight(uint imageSize) const;
    uint getCount(uint imageSize) const;
  };

  Image::Transform imageTransform; // TODO hide

protected: // corrections
  struct Pixpos {  // TODO bad names
    Pixpos(): Pixpos(0,0) {}
    Pixpos(qreal gamma, qreal tth): gammaPix(gamma), tthPix(tth) {}
    qreal gammaPix;
    qreal tthPix;
  };

  // TODO rename;
  QVector<Pixpos>      angleCorrArray;
  Borders            ful, cut;

  QPoint  getPixMiddle(uint imageSize) const;  // TODO rename, was getPixMiddleX/Y

  // TODO cashing of calcAngle...
  qreal lastCalcTthMitte; QPoint lastPixMiddle;
  qreal lastPixSpan, lastSampleDetectorSpan;
  imagecut_t lastImageCut;

  shp_File corrFile;

public:
  QVector<Pixpos> const& calcAngleCorrArray(qreal tthMitte);  // TODO rename; TODO if too slow, cache

public: // TODO not public
  Image intensCorrArray;  // summed corrFile intensities
  void calcIntensCorrArray();

  Borders const& getCut() const { return cut; }

public: // image
  imagecut_t const& getImageCut() const { return imageCut; }
  void setImageCut(bool topLeft, bool linked, imagecut_t const&);

protected:
  imagecut_t imageCut;
};

}

#endif
