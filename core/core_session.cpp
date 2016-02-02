#include "core_session.h"
#include <cmath>

namespace core {

Session::Session()
: dataFiles(), imageSize(0)
, pixSpan(0.01), sampleDetectorSpan(1.0) // TODO these must be reasonable limited
, hasBeamOffset(false), middlePixXOffset(0), middlePixYOffset(0)
, imageTransform(Image::Transform::NONE)
, lastCalcTthMitte(0) {
}

Session::~Session() {
}

uint Session::numFiles(bool withCorr) {
  return dataFiles.count() + (withCorr && hasCorrFile() ? 1 : 0);
}

void Session::addFile(rcstr fileName) THROWS {
  if (hasFile(fileName)) return;

  shp_File file(new File(fileName));
  file->load();

  setImageSize(file->getImageSize());

  dataFiles.append(file);
}

bool Session::hasFile(rcstr fileName) {
  QFileInfo fileInfo(fileName);
  for (auto& file: dataFiles)
    if (fileInfo == file->getInfo()) return true;
  return false;
}

File const& Session::getFile(uint i) {
  if ((uint)dataFiles.count() == i) {
    ASSERT(!corrFile.isNull())
    return *corrFile;
  } else {
    return *dataFiles.at(i);
  }
}

void Session::remFile(uint i) {
  if ((uint)dataFiles.count() == i) {
    corrFile.clear();
  } else {
    dataFiles.remove(i);
  }

  updateImageSize();
}

bool Session::hasCorrFile() const {
  return !corrFile.isNull();
}

void Session::loadCorrFile(rcstr fileName) {
  if (fileName.isEmpty()) return;

  shp_File file(new File(fileName));
  file->load();
  file->fold();
  corrFile = file;
  setImageSize(file->getImageSize());
  calcIntensCorrArray();
}

void Session::setImageSize(uint size) THROWS {
  if (0 == size)
    THROW("inconsistent image size");
  if (0 == imageSize)
    imageSize = size;
  else if (imageSize != size)
    THROW(" session-inconsistent image size");
}

void Session::updateImageSize() {
  if (dataFiles.isEmpty() && !hasCorrFile())
    imageSize = 0;
}

void Session::setMirror(bool on) {
  imageTransform = imageTransform.mirror(on);
}

void Session::setRotate(core::Image::Transform rot) {
  imageTransform = imageTransform.rotateTo(rot);
}

QPoint Session::getPixMiddle(uint imageSize) const {
  QPoint middle( // TODO hasBeamOffset
    imageSize / 2 + middlePixXOffset,
    imageSize / 2 + middlePixYOffset);
  // TODO was: if ((tempPixMiddleX *[<=]* 0) || (tempPixMiddleX >= getWidth()))
  // TODO this limitation could be maybe lifted (think small angle X-ray scattering?)
  RUNTIME_CHECK(Interval(0,imageSize).contains(middle.x()), "bad pixMiddle");
  RUNTIME_CHECK(Interval(0,imageSize).contains(middle.y()), "bad pixMiddle");
  return middle;
}

// TODO this is a slightly modified original code; be careful; eventually refactor
QVector<Session::Pixpos> const& Session::calcAngleCorrArray(qreal tthMitte) {

  uint count = imageSize * imageSize;
  QPoint pixMiddle = getPixMiddle(imageSize);

  if (angleCorrArray.count() == (int)count
      && lastCalcTthMitte==tthMitte && lastPixMiddle == pixMiddle
      && lastPixSpan == pixSpan && lastSampleDetectorSpan == sampleDetectorSpan
      && lastImageCut == imageCut)
    return angleCorrArray;

  angleCorrArray.resize(count);
  lastCalcTthMitte = tthMitte; lastPixMiddle = pixMiddle;
  lastPixSpan = pixSpan; lastSampleDetectorSpan = sampleDetectorSpan;
  lastImageCut = imageCut;

  auto angleCorr = [this](int x, int y) {
    return &angleCorrArray[Image::index(imageSize,x,y)];
  };

  ful.tth_regular.set(tthMitte);
  ful.tth_gamma0.set(tthMitte);
  ful.gamma.set(0);

  ASSERT(pixSpan>0) // TODO
  ASSERT(sampleDetectorSpan>0) // TODO

  // Fill the Array
  for (uint i = 0; i < imageSize; i++) {
    int abstandInPixVertical = pixMiddle.y() - i;
    qreal y = abstandInPixVertical * pixSpan;
    for (uint j = 0; j < imageSize; j++) {
      // TTH des Pixels berechnen
      int abstandInPixHorizontal = - pixMiddle.x() + j;
      qreal x = abstandInPixHorizontal * pixSpan;
      qreal z = hypot(x,y);
      qreal detektorAbstandPixel = hypot(z,sampleDetectorSpan);
      qreal tthHorAktuell = tthMitte + atan(x / sampleDetectorSpan);
      qreal detektorAbstandHorPixel = hypot(x,sampleDetectorSpan);
      qreal h = cos(tthHorAktuell) * detektorAbstandHorPixel;
      qreal tthNeu = acos(h / detektorAbstandPixel);

      // Gamma des Pixels berechnen
      qreal r = sqrt((detektorAbstandPixel * detektorAbstandPixel) - (h * h));
      qreal gamma = asin(y / r);

      // Check: if tthNeu negativ
      if (tthHorAktuell < 0) {
        tthNeu *= -1;
        gamma *= -1;
      }

      // Maxima und minima setzen
      ful.gamma.extend(gamma);
      ful.tth_regular.extend(tthNeu);
      ful.tth_gamma0.extend(tthHorAktuell);

      // Write angle in array
      *angleCorr(j,i) = Pixpos(gamma,tthNeu);
    }
  }

  // Calculate Gamma and TTH after cut
  // TODO the original code called setPixCut - is used elsewhere?
  // TODO refactor
  int arrayMiddleX = imageCut.left + imageCut.getWidth(imageSize)  / 2;
  int arrayMiddleY = imageCut.top  + imageCut.getHeight(imageSize) / 2;

  Pixpos middlePoint = *angleCorr(arrayMiddleX,arrayMiddleY);
  cut.gamma.set(middlePoint.gammaPix);
  cut.tth_regular.set(middlePoint.tthPix);
  cut.tth_gamma0.safeSet(
    angleCorr(imageSize - 1 - imageCut.right,pixMiddle.y())->tthPix,
    angleCorr(imageCut.left,pixMiddle.y())->tthPix);

  for (uint i = imageCut.left; i < imageSize - imageCut.right; i++) {
    for (uint j = imageCut.top; j < imageSize - imageCut.bottom; j++) {
      auto ac = angleCorr(i,j);
      cut.gamma.extend(ac->gammaPix);
      cut.tth_regular.extend(ac->tthPix);
    }
  }

  return angleCorrArray;
}

void Session::calcIntensCorrArray() {
  if (!hasCorrFile()) {
    intensCorrArray.clear();
    return;
  }

  ASSERT(1 == corrFile->numDatasets()) // no need to sum
  Image const& image = corrFile->getDataset(0).getImage();

  qreal sum = 0; uint n = 0;
  for (uint x=imageCut.left; x<imageSize-imageCut.right; ++x)
    for (uint y=imageCut.top; y<imageSize-imageCut.bottom; ++y) {
      sum += image.intensity(imageTransform,x,y);
      ++n; // TODO aren't we lazy
    }

  ASSERT(n>0)
  qreal avg = sum / n;

  intensCorrArray.fill(1,imageSize);
  for (uint x=imageCut.left; x<imageSize-imageCut.right; ++x)
    for (uint y=imageCut.top; y<imageSize-imageCut.bottom; ++y) {
      qreal val = avg / image.intensity(imageTransform,x,y); // TODO /0 -> inf -> nan
      if (qIsInf(val)) val = 1; // TODO verify
      intensCorrArray.setIntensity(imageTransform,x,y,val);
    }
}

Session::imagecut_t::imagecut_t(uint top_, uint bottom_, uint left_, uint right_)
  : top(top_), bottom(bottom_), left(left_), right(right_) {
}

bool Session::imagecut_t::operator==(Session::imagecut_t const& that) {
  return top==that.top && bottom==that.bottom && left==that.left && right==that.right;
}

uint Session::imagecut_t::getWidth(uint imageSize) const {
  return imageSize - left - right;
}

uint Session::imagecut_t::getHeight(uint imageSize) const {
  return imageSize - top - bottom;
}

uint Session::imagecut_t::getCount(uint imageSize) const {
  return getWidth(imageSize) * getHeight(imageSize);
}

void Session::setImageCut(bool topLeft, bool linked, imagecut_t const& imageCut_) {
  if (0 == imageSize)
    imageCut = imagecut_t();
  else {
    auto limit = [linked](uint &thisOne, uint &thatOne, uint maxTogether) {
      if (linked && thisOne+thatOne>=maxTogether) {
        thisOne = thatOne = qMax(0u, (maxTogether-1) / 2);
      } else {
        thisOne = qMax(qMin(thisOne, maxTogether - thatOne - 1), 0u);
        thatOne = qMax(qMin(thatOne, maxTogether - thisOne - 1), 0u);
      }
    };

    imageCut = imageCut_;
    // make sure that cut values are valid; in the right order
    if (topLeft) {
      limit(imageCut.top,   imageCut.bottom,  imageSize);
      limit(imageCut.left,  imageCut.right,   imageSize);
    } else {
      limit(imageCut.bottom,imageCut.top,     imageSize);
      limit(imageCut.right, imageCut.left,    imageSize);
    }
  }
}

}

// eof
