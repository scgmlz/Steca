/** \file
 * Session that can compute all and needs no GUI.
 */

#ifndef CORE_SESSION_H
#define CORE_SESSION_H

#include "core_types.h"
#include "core_file.h"
#include "core_array2d.h"
#include "approx/approx_functions.h"
#include <QPoint>

namespace core {
//------------------------------------------------------------------------------

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
#ifdef TEST_UNIT_TESTS
public:
#else
private:
#endif
  void setImageSize(QSize const&) THROWS; ///< Ensures that all images have the same size.

public: // detector geometry
  struct Geometry {
    Geometry();
    bool operator ==(Geometry const&) const;

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
  /// Image transform - rotation and mirroring
  struct ImageTransform {
    enum e {
      ROTATE_0        = 0,  // no transform
      ROTATE_1        = 1,  // one quarter
      ROTATE_2        = 2,  // two quarters
      ROTATE_3        = 3,  // three quarters
      MIRROR          = 4,
      MIRROR_ROTATE_0 = MIRROR | ROTATE_0,
      MIRROR_ROTATE_1 = MIRROR | ROTATE_1,
      MIRROR_ROTATE_2 = MIRROR | ROTATE_2,
      MIRROR_ROTATE_3 = MIRROR | ROTATE_3,
    } val;

    ImageTransform(int val = ROTATE_0);             ///< clamps val appropriately
    ImageTransform mirror(bool on)          const;  ///< adds/removes the mirror flag
    ImageTransform rotateTo(ImageTransform) const;  ///< rotates, but keeps the mirror flag
    ImageTransform nextRotate()             const;  ///< rotates by one quarter-turn

    bool isTransposed() const { return 0 != (val&1); }

    bool operator ==(ImageTransform const& that) const { return val == that.val; }
  };

protected:
  ImageTransform imageTransform;

public:
  void setImageMirror(bool);
  void setImageRotate(ImageTransform);

  /// Calculate the 1D index of a pixel, with transform.
  uint pixIndex(uint x, uint y) const;
  /// Calculate the 1D index of a pixel, no transform, only size considered.
  uint pixIndexNoTransform(uint x, uint y) const;

  /// Get intensity from an image with/without correction; *may return NaN*
  intens_t pixIntensity(Image const&, uint x, uint y) const;

  QSize getImageSize() const;

protected: // corrections TODO make private
  struct Pixpos {  // TODO bad name
    Pixpos(): Pixpos(0,0) {}
    Pixpos(qreal gamma, qreal tth): gammaPix(gamma), tthPix(tth) {}
    qreal gammaPix;
    qreal tthPix;
  };

  // TODO rename;
  typedef Array2D<Pixpos> AngleCorrArray;
  AngleCorrArray angleCorrArray;

  struct Borders {
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

  Borders        ful, cut;

  QPoint  getPixMiddle() const;  // TODO rename, was getPixMiddleX/Y

  // TODO caching of calcAngle...
  qreal lastCalcTthMitte; QPoint lastPixMiddle;
  Geometry lastGeometry;

public:
  struct ImageCut {
    ImageCut(uint top = 0, uint bottom = 0, uint left = 0, uint right = 0);
    bool operator==(ImageCut const&);
    uint top, bottom, left, right;

    uint getWidth(QSize const&) const;
    uint getHeight(QSize const&) const;
    uint getCount(QSize const&) const;
  };

protected:
  ImageCut lastImageCut;
  ImageTransform lastImageTransform;

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

//------------------------------------------------------------------------------
}
#endif
