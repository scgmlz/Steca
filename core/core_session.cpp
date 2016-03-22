#include "core_session.h"
#include "io/core_io.h"
#include "core_fit_methods.h"
#include <cmath>

namespace core {
//------------------------------------------------------------------------------

qreal const Geometry::MIN_DETECTOR_DISTANCE   = 1000;
qreal const Geometry::MIN_DETECTOR_PIXEL_SIZE = 1;

Geometry::Geometry() {
  detectorDistance  = MIN_DETECTOR_DISTANCE;
  pixSize             = MIN_DETECTOR_PIXEL_SIZE;
  hasBeamOffset       = false;
  middlePixOffset     = QPoint();
}

bool Geometry::operator ==(Geometry const& that) const {
  return
    detectorDistance == that.detectorDistance &&
    pixSize            == that.pixSize &&
    hasBeamOffset      == that.hasBeamOffset &&
    middlePixOffset    == that.middlePixOffset;
}

//------------------------------------------------------------------------------

Session::Session()
: dataFiles(), corrFile(), corrEnabled(false), imageSize(), geometry()
, imageTransform(ImageTransform::ROTATE_0)
, angleMapArray()
, lastCalcTthMitte(0)
, hasNaNs(false), bgPolynomial(0) {
}

Session::~Session() {
}

void Session::clear() {
  // TODO REVIEW
  reflections.empty();
  enableCorrection(false);
  remCorrFile();
  while (numFiles(false)) remFile(0);
}

uint Session::numFiles(bool withCorr) const {
  return dataFiles.count() + (withCorr && !corrFile.isNull() ? 1 : 0);
}

shp_File Session::addFile(rcstr fileName) THROWS {
  if (fileName.isEmpty() || hasFile(fileName))
    return shp_File(); // nichts zu tun

  shp_File file = io::load(fileName);

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

  shp_File file = io::load(fileName);
  file->fold();

  setImageSize(file->getImageSize());

  // all ok
  corrFile    = file;
  corrEnabled = true;
  calcIntensCorrArray();
  return file;
}

void Session::remCorrFile() {
  corrFile.clear();
  corrEnabled = false;
  updateImageSize();
}

shp_File Session::getCorrFile() const {
  return corrFile;
}

void Session::enableCorrection(bool on) {
  corrEnabled = on && hasCorrFile();
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

void Session::setGeometry(qreal detectorDistance, qreal pixSize, bool hasBeamOffset, QPoint const& middlePixOffset) {
  ASSERT(detectorDistance>0 && pixSize>0) // TODO use MIN_DETECTOR_DISTANCE ...
  geometry.detectorDistance = detectorDistance;
  geometry.pixSize            = pixSize;
  geometry.hasBeamOffset      = hasBeamOffset;
  geometry.middlePixOffset    = middlePixOffset;
}

void Session::setImageMirror(bool on) {
  imageTransform = imageTransform.mirror(on);
}

void Session::setImageRotate(ImageTransform rot) {
  imageTransform = imageTransform.rotateTo(rot);
}

ImageTransform Session::getImageTransform() const {
  return imageTransform;
}

QSize Session::getImageSize() const {
  return imageTransform.isTransposed()
    ? imageSize.transposed() : imageSize;
}

shp_LensSystem Session::allLenses(Dataset const& dataset,
                                  bool const globalIntensityScale) {
  auto lenses = plainLens(dataset);
  lenses << shp_LensSystem(new TransformationLens(imageTransform));
  lenses << shp_LensSystem(new ROILens(imageCut));
  if (corrEnabled)
    lenses << shp_LensSystem(new SensitivityCorrectionLens(intensCorrArray));
  if (globalIntensityScale)
    lenses << shp_LensSystem(
        new GlobalIntensityRangeLens(dataset.getFile().getRgeIntens()));
  else
    lenses << shp_LensSystem(new IntensityRangeLens());
  return lenses;
}

shp_LensSystem Session::noROILenses(Dataset const& dataset,
                                    bool const globalIntensityScale) {
  auto lenses = plainLens(dataset);
  lenses << shp_LensSystem(new TransformationLens(imageTransform));
  if (corrEnabled)
    lenses << shp_LensSystem(new SensitivityCorrectionLens(intensCorrArray));
  if (globalIntensityScale)
    lenses << shp_LensSystem(
        new GlobalIntensityRangeLens(dataset.getFile().getRgeIntens()));
  else
    lenses << shp_LensSystem(new IntensityRangeLens());
  return lenses;
}

shp_LensSystem Session::plainLens(Dataset const& dataset) {
  return makeLensSystem(dataset, calcAngleMap(dataset.tthMitte()));
}

QPoint Session::getPixMiddle() const {
  auto imageSize = getImageSize();
  QPoint middle( // REVIEW TODO when hasBeamOffset
    imageSize.width()  / 2 + geometry.middlePixOffset.x(),
    imageSize.height() / 2 + geometry.middlePixOffset.y());
  // TODO was: if ((tempPixMiddleX *[<=]* 0) || (tempPixMiddleX >= getWidth()))
  // TODO this limitation could be maybe lifted (think small angle X-ray scattering?)
  RUNTIME_CHECK(Range(0,imageSize.width()).contains(middle.x()), "bad pixMiddle");
  RUNTIME_CHECK(Range(0,imageSize.height()).contains(middle.y()), "bad pixMiddle");
  return middle;
}

AngleMapArray const& Session::calcAngleMap(qreal tthMitte) { // RENAME
  // REFACTOR
  QPoint pixMiddle = getPixMiddle();
  auto size   = getImageSize();

  if (angleMapArray.getSize() == size
      && lastCalcTthMitte==tthMitte && lastPixMiddle == pixMiddle
      && lastGeometry == geometry && lastImageCut == imageCut
      && lastImageTransform == imageTransform)
    return angleMapArray;

  lastCalcTthMitte = tthMitte; lastPixMiddle = pixMiddle;
  lastGeometry = geometry;
  lastImageCut = imageCut;
  lastImageTransform = imageTransform;

  angleMapArray.fill(size);
  ful.invalidate();
  cut.invalidate();

  if (!size.isEmpty()) {
    ASSERT(geometry.pixSize>0) // TODO better than asserts
    ASSERT(geometry.detectorDistance>0)

    // Fill the Array
    for (int iy = 0; iy < size.height(); ++iy) {
      int abstandInPixVertical = pixMiddle.y() - iy;
      qreal y = abstandInPixVertical * geometry.pixSize;
      for (int ix = 0; ix < size.width(); ++ix) {
        // TTH des Pixels berechnen
        int abstandInPixHorizontal = - pixMiddle.x() + ix;
        qreal x = abstandInPixHorizontal * geometry.pixSize;
        qreal z = hypot(x,y);
        qreal detektorAbstandPixel = hypot(z,geometry.detectorDistance);
        qreal tthHorAktuell = tthMitte + atan(x / geometry.detectorDistance);
        qreal detektorAbstandHorPixel = hypot(x,geometry.detectorDistance);
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
        angleMapArray.setAt(ix, iy, DiffractionAngles(gamma,tthNeu));
      }
    }

    ASSERT(ful.isValid())

    // Calculate Gamma and TTH after cut
    ASSERT(imageCut.getCount(size) > 0)

    cut.tth_gamma0.safeSet(
      angleMapArray.at(size.width() - 1 - imageCut.right,pixMiddle.y()).tth,
      angleMapArray.at(imageCut.left,pixMiddle.y()).tth);

    // TODO loop through cut - make abstract (see below), REFACTOR
    for (uint ix = imageCut.left; ix < size.width() - imageCut.right; ++ix) {
      for (uint iy = imageCut.top; iy < size.height() - imageCut.bottom; ++iy) {
        auto ac = angleMapArray.at(ix,iy);
        cut.gamma.extend(ac.gamma);
        cut.tth_regular.extend(ac.tth);
      }
    }

    ASSERT(cut.isValid())
  }

  return angleMapArray;
}

void Session::calcIntensCorrArray() {
  hasNaNs = false;

  if (corrFile.isNull()) {
    intensCorrArray.clear();
    return;
  }

  ASSERT(1 == corrFile->numDatasets()) // no need to sum
  auto lenses = plainLens(*corrFile->getDataset(0));
  lenses << shp_LensSystem(new ROILens(imageCut));

  // REVIEW
  qreal sum = 0; uint n = 0;
  auto size = lenses->getSize();
  for (int iy = 0; iy < size.height(); ++iy) {
    for (int ix = 0; ix < size.width(); ++ix) {
      sum += lenses->getIntensity(ix, iy);
      ++n;
    }
  }

  ASSERT(n>0) // TODO div. by 0 ?
  qreal avg = sum / n;

  intensCorrArray.fill(1,imageSize);
  for (int iy = 0; iy < size.height(); ++iy)
    for (int ix = 0; ix < size.width(); ++ix) {
      auto intens = lenses->getIntensity(ix, iy);
      qreal val;

      if (intens>0) {
        val = avg / intens;
      } else {
        val = qQNaN(); hasNaNs = true;
      }

      intensCorrArray.setAt(ix + imageCut.left, iy + imageCut.top, val);
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

NormalizationLens Session::makeNormalizationLens(Dataset const& dataset) {
  File parentFile = dataset.getFile();
  qreal normVal = 0;
  qreal average = 0;
  qreal current = 1;
  switch (Normalization::NUM_NORM_TYPES) {
  case Normalization::DELTA_TIME:
    average = parentFile.calAverageDeltaTime();
    current = dataset.getNumericalAttributeValue(core::Dataset::eAttributes::DELTA_TIME);
    break;
  case Normalization::MON_COUNTS:
    average = parentFile.calAverageMonitor();
    current = dataset.getNumericalAttributeValue(core::Dataset::eAttributes::MON);
    break;
  case Normalization::BG_LEVEL:
    
    break;
  default:
    NEVER_HERE
  }
  normVal = average/current;
  return NormalizationLens(normVal);
}

//------------------------------------------------------------------------------
}
// eof
