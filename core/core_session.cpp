#include "core_session.h"
#include "approx/approx_methods.h"
#include <cmath>

namespace core {

Session::Session()
: dataFiles(), corrFile(), imageSize(), geometry()
, imageTransform(ImageTransform::ROTATE_0)
, lastCalcTthMitte(0), hasNaNs(false) {

#ifdef DEVELOPMENT
  core::Ranges ranges;
  ranges.add(Range(-1000,1000));
  core::TI_Data tiData;
  tiData.append(0,0);
  tiData.append(1,1);
  tiData.append(2,0);
  calcBGCorrectionPolynomial(ranges,tiData);
#endif
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

void Session::updateImageSize() {
  if (0 == numFiles(true))
    imageSize = QSize(0,0);
}

void Session::setImageSize(QSize const& size) THROWS {
  RUNTIME_CHECK (!size.isEmpty(), "bad image size");
  if (imageSize.isEmpty()) // the first one
    imageSize = size;
  else if (imageSize != size)
    THROW("inconsistent image size");
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

void Session::setGeometry(qreal sampleDetectorSpan, qreal pixSpan, bool hasBeamOffset, QPoint const& middlePixOffset) {
  geometry.sampleDetectorSpan = sampleDetectorSpan;
  geometry.pixSpan            = pixSpan;
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

uint Session::pixIndexNoTransform(uint x, uint y) const {
  uint w = imageTransform.isTransposed()
    ? imageSize.height() : imageSize.width();
  return x + y * w;
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

  lastCalcTthMitte = tthMitte; lastPixMiddle = pixMiddle;
  lastGeometry = geometry;
  lastImageCut = imageCut;
  lastImageTransform = imageTransform;

  angleCorrArray.fill(size);
  ful.invalidate();
  cut.invalidate();

  if (!size.isEmpty()) {
    ASSERT(geometry.pixSpan>0) // TODO
    ASSERT(geometry.sampleDetectorSpan>0) // TODO

    // Fill the Array
    for (int iy = 0; iy < size.height(); ++iy) {
      int abstandInPixVertical = pixMiddle.y() - iy;
      qreal y = abstandInPixVertical * geometry.pixSpan;
      for (int ix = 0; ix < size.width(); ++ix) {
        // TTH des Pixels berechnen
        int abstandInPixHorizontal = - pixMiddle.x() + ix;
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
          tthNeu = -tthNeu;
          gamma  = -gamma;
        }

        // Maxima und minima setzen
        ful.gamma.extend(gamma);
        ful.tth_regular.extend(tthNeu);
        ful.tth_gamma0.extend(tthHorAktuell);

        // Write angle in array
        angleCorrArray.setAt(pixIndexNoTransform(ix,iy),Pixpos(gamma,tthNeu));
      }
    }

    ASSERT(ful.isValid())

    // Calculate Gamma and TTH after cut
    // TODO the original code called setPixCut - is used elsewhere?
    // TODO refactor
    ASSERT(imageCut.getCount(size) > 0)

    cut.tth_gamma0.safeSet(
      angleCorrArray.at(size.width() - 1 - imageCut.right,pixMiddle.y()).tthPix,
      angleCorrArray.at(imageCut.left,pixMiddle.y()).tthPix);

    // TODO loop through cut - abstract
    for (uint ix = imageCut.left; ix < size.width() - imageCut.right; ++ix) {
      for (uint iy = imageCut.top; iy < size.height() - imageCut.bottom; ++iy) {
        auto ac = angleCorrArray.at(ix,iy);
        cut.gamma.extend(ac.gammaPix);
        cut.tth_regular.extend(ac.tthPix);
      }
    }

    ASSERT(cut.isValid())
  }

  return angleCorrArray;
}

approx::Polynomial Session::calcBGCorrectionPolynomial(Ranges const& ranges,TI_Data const& vecSpec) {
  if (ranges.isEmpty())
    return approx::Polynomial();

  approx::Curve curve;

  ASSERT(vecSpec.isOrdered())

  auto tth   = vecSpec.getTth();
  auto inten = vecSpec.getInten();

  uint i = 0, count = tth.count();
  for (auto const& range: ranges.getData()) {
    while (i<count && tth[i] <  range.min)
      ++i;
    while (i<count && tth[i] <= range.max) {
      curve.addPoint(approx::Curve::Point(tth[i],inten[i]));
      ++i;
    }
  }

  approx::Polynomial polynomial;

  approx::FittingLevenbergMarquardt approximation(polynomial);
  approximation.fitWithoutCheck(curve);

  return polynomial;
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
