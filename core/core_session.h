#ifndef CORE_SESSION_H
#define CORE_SESSION_H

#include "core_defs.h"
#include "core_file.h"
#include <QSize>
#include <QPointF>

namespace core {

class Session {
public:
  Session();
 ~Session();

  uint numFiles(bool withCorr=false);

  void addFile(rcstr fileName) THROWS;
  bool hasFile(rcstr fileName);

  File const& getFile(uint i);
  void remFile(uint i);

  bool hasCorrFile()   const;
  void setCorrFile(rcstr fileName); // fileName may be empty -> unsets TODO

protected:
  Files dataFiles;
  QSharedPointer<File> corrFile;

  uint imageSize;

  void setImageSize(uint) THROWS;
  void updateImageSize();

public: // detector TODO make a structure; rename variables
  qreal pixSpan;            // size of the detector pixel
  qreal sampleDetectorSpan; // the distance between sample - detector // TODO verify: in adhoc has at least three names: sampleDetectorSpan, detectorSampleSpan, detectorSampleDistance
  bool  hasBeamOffset;
  int   middlePixXOffset;
  int   middlePixYOffset;

public: // image transform
  void setUpDown(bool);
  void setLeftRight(bool);
  void setTurnClock(bool);
  void setTurnCounter(bool);

private:
  friend class Image; // TODO this is for Image::index(), do better
  bool upDown, leftRight, turnClock, turnCounter;

protected: // corrections
  struct Pixpos {  // TODO bad names
    Pixpos(): Pixpos(0,0) {}
    Pixpos(qreal gamma, qreal tth): gammaPix(gamma), tthPix(tth) {}
    qreal gammaPix;
    qreal tthPix;
  };

  // TODO rename; TODO share these by (tth,image size)
  QVector<Pixpos>      angleCorrArray;
  borders_t            ful, cut;

  QPoint  getPixMiddle(Image const&) const;  // TODO rename, was getPixMiddleX/Y
  void    createAngleCorrArray(Image const&,qreal);  // TODO rename to "map"; TODO if too slow, cache

  float** intensCorrArray = NULL;
  float** intensCorrArrayNotStandardized = NULL;

public: // image
  struct imagecut_t {
    imagecut_t(uint top = 0, uint bottom = 0, uint left = 0, uint right = 0);
    uint top, bottom, left, right;
  };

  imagecut_t const& getImageCut() const { return imageCut; }
  void setImageCut(bool topLeft, bool linked, imagecut_t const&);

protected:
  imagecut_t imageCut;
};

}

#endif
