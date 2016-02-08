#include "core_session.h"
#include <cmath>

namespace core {

Session::Session()
: dataFiles(), corrFile(), imageSize(), geometry()
, imageTransform(ImageTransform::NONE)
, lastCalcTthMitte(0), hasNaNs(false) {
}

Session::~Session() {
}

uint Session::numFiles(bool withCorr) const {
  return dataFiles.count() + (withCorr && !corrFile.isNull() ? 1 : 0);
}

shp_File Session::addFile(rcstr fileName) THROWS {
  if (fileName.isEmpty() || hasFile(fileName))
    return shp_File(); // nichts zu tun

  shp_File file(new File(fileName));
  file->load();

  setImageSize(file->getImageSize());

  // all ok
  dataFiles.append(file);
  return file;
}

shp_File Session::remFile(uint i) {
  shp_File file = dataFiles.at(i);
  dataFiles.remove(i);
  updateImageSize();
  return file;
}

bool Session::hasFile(rcstr fileName) {
  QFileInfo fileInfo(fileName);
  for (auto &file: dataFiles)
    if (fileInfo == file->getInfo()) return true;
  return false;
}

shp_File Session::getFile(uint i) const {
  if ((uint)dataFiles.count() == i) {
    ASSERT(!corrFile.isNull())
    return corrFile;
  } else {
    return dataFiles.at(i);
  }
}

shp_File Session::loadCorrFile(rcstr fileName) {
  if (fileName.isEmpty()) {
    remCorrFile();
    return shp_File();
  }

  shp_File file(new File(fileName));
  file->load();
  file->fold();

  setImageSize(file->getImageSize());

  // all ok
  corrFile = file;
  calcIntensCorrArray();
  return file;
}

void Session::remCorrFile() {
  corrFile.clear();
  updateImageSize();
}

shp_File Session::getCorrFile() const {
  return corrFile;
}

void Session::setImageSize(QSize const& size) THROWS {
  RUNTIME_CHECK (!size.isEmpty(), "bad image size");
  if (imageSize.isEmpty()) // the first one
    imageSize = size;
  else if (imageSize != size)
    THROW("inconsistent image size");
}

void Session::updateImageSize() {
  if (0 == numFiles(true))
    imageSize = QSize(0,0);
}

Session::Geometry::Geometry() {
  sampleDetectorSpan  = 1.0;
  pixSpan             = 0.01; // TODO these must be reasonable limited
  hasBeamOffset       = false;
  middlePixOffset     = QPoint();
}

bool Session::Geometry::operator ==(Geometry const& that) const {
  return
    sampleDetectorSpan == that.sampleDetectorSpan &&
    pixSpan            == that.pixSpan &&
    hasBeamOffset      == that.hasBeamOffset &&
    middlePixOffset    == that.middlePixOffset;
}

void Session::setDetectorGeometry(qreal sampleDetectorSpan, qreal pixSpan) {
  geometry.sampleDetectorSpan = sampleDetectorSpan;
  geometry.pixSpan            = pixSpan;
}

void Session::setBeamGeometry(bool hasBeamOffset, QPoint const& middlePixOffset) {
  geometry.hasBeamOffset      = hasBeamOffset;
  geometry.middlePixOffset    = middlePixOffset;
}

Session::ImageTransform::ImageTransform(int val_): val((e)(val_ & 7)) {
}

Session::ImageTransform Session::ImageTransform::mirror(bool on) const {
  return on ? ImageTransform(val |  MIRROR)
            : ImageTransform(val & ~MIRROR);
}

Session::ImageTransform Session::ImageTransform::rotateTo(ImageTransform rot) const {
  return ImageTransform((val & MIRROR) | (rot.val & 3));
}

Session::ImageTransform Session::ImageTransform::nextRotate() const {
  return rotateTo(val+1);
}

void Session::setImageMirror(bool on) {
  imageTransform = imageTransform.mirror(on);
}

void Session::setImageRotate(ImageTransform rot) {
  imageTransform = imageTransform.rotateTo(rot);
}

uint Session::pixIndex(uint x, uint y) const {

  // imageSize is not transformed
  uint w = imageSize.width(), h = imageSize.height();

  switch (imageTransform.val) {
  case ImageTransform::ROTATE_0:
    return x + y * w;
  case ImageTransform::ROTATE_1:
    x = h - x - 1;
    return y + x * w;
  case ImageTransform::ROTATE_2:
    x = w - x - 1;
    y = h - y - 1;
    return x + y * w;
  case ImageTransform::ROTATE_3:
    y = w - y - 1;
    return y + x * w;
  case ImageTransform::MIRROR_ROTATE_0:
    x = w - x - 1;
    return x + y * w;
  case ImageTransform::MIRROR_ROTATE_1:
    x = h - x - 1;
    y = w - y - 1;
    return y + x * w;
  case ImageTransform::MIRROR_ROTATE_2:
    y = h - y - 1;
    return x + y * w;
  case ImageTransform::MIRROR_ROTATE_3:
    return y + x * w;
  }
  NOT_HERE
}

QSize Session::getImageSize() const {
  return imageTransform.isTransposed()
    ? imageSize.transposed() : imageSize;
}


QPoint Session::getPixMiddle() const {
  auto imageSize = getImageSize();
  QPoint middle( // TODO hasBeamOffset
    imageSize.width()  / 2 + geometry.middlePixOffset.x(),
    imageSize.height() / 2 + geometry.middlePixOffset.y());
  // TODO was: if ((tempPixMiddleX *[<=]* 0) || (tempPixMiddleX >= getWidth()))
  // TODO this limitation could be maybe lifted (think small angle X-ray scattering?)
  RUNTIME_CHECK(Range(0,imageSize.width()).contains(middle.x()), "bad pixMiddle");
  RUNTIME_CHECK(Range(0,imageSize.height()).contains(middle.y()), "bad pixMiddle");
  return middle;
}

// TODO this is a slightly modified original code; be careful; eventually refactor
Session::AngleCorrArray const& Session::calcAngleCorrArray(qreal tthMitte) {

  QPoint pixMiddle = getPixMiddle();
  auto size   = getImageSize();

  if (angleCorrArray.getSize() == size
      && lastCalcTthMitte==tthMitte && lastPixMiddle == pixMiddle
      && lastGeometry == geometry && lastImageCut == imageCut
      && lastImageTransform == imageTransform)
    return angleCorrArray;

  angleCorrArray.fill(size);

  lastCalcTthMitte = tthMitte; lastPixMiddle = pixMiddle;
  lastGeometry = geometry;
  lastImageCut = imageCut;
  lastImageTransform = imageTransform;

  ful.tth_regular.set(tthMitte);
  ful.tth_gamma0.set(tthMitte);
  ful.gamma.set(0);

  if (size.isEmpty()) {
    // TODO set to something - needed? or invalidate() ?
    cut.gamma.set(0);
    cut.tth_regular.set(0);
    cut.tth_gamma0.set(0);
  } else {
    ASSERT(geometry.pixSpan>0) // TODO
    ASSERT(geometry.sampleDetectorSpan>0) // TODO

    // Fill the Array
    for (int i = 0; i < size.height(); i++) {
      int abstandInPixVertical = pixMiddle.y() - i;
      qreal y = abstandInPixVertical * geometry.pixSpan;
      for (int j = 0; j < size.width(); j++) {
        // TTH des Pixels berechnen
        int abstandInPixHorizontal = - pixMiddle.x() + j;
        qreal x = abstandInPixHorizontal * geometry.pixSpan;
        qreal z = hypot(x,y);
        qreal detektorAbstandPixel = hypot(z,geometry.sampleDetectorSpan);
        qreal tthHorAktuell = tthMitte + atan(x / geometry.sampleDetectorSpan);
        qreal detektorAbstandHorPixel = hypot(x,geometry.sampleDetectorSpan);
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
        angleCorrArray.setAt(pixIndex(j,i),Pixpos(gamma,tthNeu));
      }
    }

    // Calculate Gamma and TTH after cut
    // TODO the original code called setPixCut - is used elsewhere?
    // TODO refactor
    int arrayMiddleX = imageCut.left + imageCut.getWidth(size)   / 2;
    int arrayMiddleY = imageCut.top  + imageCut.getHeight(size) / 2;

    auto middlePoint = angleCorrArray.at(arrayMiddleX,arrayMiddleY);
    cut.gamma.set(middlePoint.gammaPix);
    cut.tth_regular.set(middlePoint.tthPix);
    cut.tth_gamma0.safeSet(
      angleCorrArray.at(size.width() - 1 - imageCut.right,pixMiddle.y()).tthPix,
      angleCorrArray.at(imageCut.left,pixMiddle.y()).tthPix);

    for (uint i = imageCut.left; i < size.width() - imageCut.right; i++) {
      for (uint j = imageCut.top; j < size.height() - imageCut.bottom; j++) {
        auto ac = angleCorrArray.at(i,j);
        cut.gamma.extend(ac.gammaPix);
        cut.tth_regular.extend(ac.tthPix);
      }
    }
  }

  return angleCorrArray;
}

void Session::calcIntensCorrArray() {
  hasNaNs = false;

  if (corrFile.isNull()) {
    intensCorrArray.clear();
    return;
  }

  ASSERT(1 == corrFile->numDatasets()) // no need to sum
  Image const& image = corrFile->getDataset(0)->getImage();

  qreal sum = 0; uint n = 0; auto size = getImageSize();
  for (uint x=imageCut.left; x<size.width()-imageCut.right; ++x)
    for (uint y=imageCut.top; y<size.height()-imageCut.bottom; ++y) {
      sum += image.intensity(pixIndex(x,y));
      ++n; // TODO aren't we lazy
    }

  ASSERT(n>0)
  qreal avg = sum / n;

  intensCorrArray.fill(1,size);
  for (uint x=imageCut.left; x<size.width()-imageCut.right; ++x)
    for (uint y=imageCut.top; y<size.height()-imageCut.bottom; ++y) {
      auto intens = image.intensity(pixIndex(x,y));
      qreal val;

      if (intens>0) {
        val = avg / intens;
      } else {
        val = qQNaN(); hasNaNs = true;
      }

      intensCorrArray.setIntensity(pixIndex(x,y),val);
    }
}

void Session::setImageCut(bool topLeft, bool linked, ImageCut const& imageCut_) {
  auto size = getImageSize();

  if (size.isEmpty())
    imageCut = ImageCut();
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
      limit(imageCut.top,   imageCut.bottom,  size.height());
      limit(imageCut.left,  imageCut.right,   size.width());
    } else {
      limit(imageCut.bottom,imageCut.top,     size.height());
      limit(imageCut.right, imageCut.left,    size.width());
    }
  }
}

}

// eof
