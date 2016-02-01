#ifndef CORE_SESSION_H
#define CORE_SESSION_H

#include "core_defs.h"
#include "core_file.h"
#include <QSize>
#include <QPointF>

namespace core {

// TODO goes elsewhere
struct interval_t { // closed interval
  interval_t(qreal low = 0, qreal hig = 0);
  qreal low, hig;

  void set(qreal);
  void set(qreal,qreal);
  void safeSet(qreal,qreal);  // in the right order

  void include(qreal);
  bool contains(qreal val) const { return low <= val && val <= hig; }
};

struct borders_t {
  interval_t
    gamma,
    tth_regular,
    tth_gamma0; // at gamma=0
};

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
  void loadCorrFile(rcstr fileName); // fileName may be empty -> unsets TODO

protected:
  Files dataFiles;

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

public: // image
  struct imagecut_t {
    imagecut_t(uint top = 0, uint bottom = 0, uint left = 0, uint right = 0);
    bool operator==(imagecut_t const&);
    uint top, bottom, left, right;

    uint getWidth(uint imageSize) const;
    uint getHeight(uint imageSize) const;
    uint getCount(uint imageSize) const;
  };

private:
  friend class Intensities; // TODO this is for Image::index(), do better
  bool upDown, leftRight, turnClock, turnCounter;

protected: // corrections
  struct Pixpos {  // TODO bad names
    Pixpos(): Pixpos(0,0) {}
    Pixpos(qreal gamma, qreal tth): gammaPix(gamma), tthPix(tth) {}
    qreal gammaPix;
    qreal tthPix;
  };

  // TODO rename;
  QVector<Pixpos>      angleCorrArray;
  borders_t            ful, cut;

  QPoint  getPixMiddle(uint imageSize) const;  // TODO rename, was getPixMiddleX/Y

  // TODO cashing of calcAngle...
  qreal lastCalcTthMitte; QPoint lastPixMiddle;
  qreal lastPixSpan, lastSampleDetectorSpan;
  imagecut_t lastImageCut;

  QSharedPointer<File> corrFile;

public:
  QVector<Pixpos> const& calcAngleCorrArray(qreal tthMitte);  // TODO rename; TODO if too slow, cache

public: // TODO not public
  Intensities intensCorrArray;  // summed corrFile intensities
  void calcIntensCorrArray();

  borders_t const& getCut() const { return cut; }

public: // image
  imagecut_t const& getImageCut() const { return imageCut; }
  void setImageCut(bool topLeft, bool linked, imagecut_t const&);

protected:
  imagecut_t imageCut;
};

}

#endif
