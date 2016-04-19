// ************************************************************************** //
//
//  STeCa2:    StressTexCalculator ver. 2
//
//! @file      core_session.cpp
//!
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016
//! @authors   Scientific Computing Group at MLZ Garching
//! @authors   Original version: Christian Randau
//! @authors   Version 2: Antti Soininen, Jan Burle, Rebecca Brydon
//
// ************************************************************************** //

#include "core_session.h"
#include "core_fit_fitting.h"
#include "types/core_type_curve.h"

namespace core {
//------------------------------------------------------------------------------

Session::Session() {
  clear();
}

void Session::clear() {
  while (0 < numFiles()) remFile(0);

  remCorrFile();
  corrEnabled_ = false;

  bgPolynomialDegree_ = 0;
  bgRanges_.clear();

  reflections_.clear();

  norm_ = Lens::normNONE;
}

shp_File Session::file(uint i) const {
  if ((uint)files_.count() == i) {
    // past the vector end, must be the correction file
    ASSERT(!corrFile_.isNull())
    return corrFile_;
  } else {
    return files_.at(i);
  }
}

bool Session::hasFile(rcstr fileName) {
  QFileInfo fileInfo(fileName);
  for (auto &file: files_)
    if (fileInfo == file->fileInfo()) return true;
  return false;
}

void Session::addFile(shp_File file) THROWS {
  setImageSize(file->datasets().imageSize());
  // all ok
  files_.append(file);
}

void Session::remFile(uint i) {
  files_.remove(i);
  updateImageSize();
}

void Session::setCorrFile(shp_File file) THROWS {
  if (file.isNull()) {
    remCorrFile();
  } else {
    auto &datasets = file->datasets();

    datasets.fold(); // ensure one dataset
    setImageSize(datasets.imageSize());

    // all ok
    corrFile_    = file;
    corrEnabled_ = true;
  }
}

void Session::remCorrFile() {
  corrFile_.clear();
  corrEnabled_ = false;
  updateImageSize();
}

void Session::enableCorr(bool on) {
  corrEnabled_ = on && hasCorrFile();
}

void Session::updateImageSize() {
  if (0 == numFiles() && !hasCorrFile())
    imageSize_ = QSize(0,0);
}

void Session::setImageSize(QSize const& size) THROWS {
  RUNTIME_CHECK (!size.isEmpty(), "bad image size");
  if (imageSize_.isEmpty())
    imageSize_ = size; // the first one
  else if (imageSize_ != size)
    THROW("inconsistent image size");
}

QSize Session::imageSize() const {
  return imageTransform_.isTransposed()
  ? imageSize_.transposed() : imageSize_;
}

void Session::setImageTransformMirror(bool on) {
  imageTransform_ = imageTransform_.mirror(on);
}

void Session::setImageTransformRotate(ImageTransform const& rot) {
  imageTransform_ = imageTransform_.rotateTo(rot);
}

void Session::setImageCut(bool topLeftFirst, bool linked, ImageCut const& cut) {
  auto size = imageSize_;

  if (size.isEmpty())
    imageCut_ = ImageCut();
  else {
    auto limit = [linked](int& m1, int& m2, int maxTogether) {
      if (linked && m1+m2 >= maxTogether) {
        m1 = m2 = qMax(0, (maxTogether-1) / 2);
      } else {
        m1 = qMax(qMin(m1, maxTogether - m2 - 1), 0);
        m2 = qMax(qMin(m2, maxTogether - m1 - 1), 0);
      }
    };

    // make sure that cut values are valid; in the right order
    int left  = cut.left,  top    = cut.top,
        right = cut.right, bottom = cut.bottom;

    if (topLeftFirst) {
      limit(top,  bottom, size.height());
      limit(left, right,  size.width());
    } else {
      limit(bottom, top,  size.height());
      limit(right,  left, size.width());
    }

    imageCut_ = ImageCut(left,top,right,bottom);
  }
}

AngleMap const& Session::angleMap(rcDataset dataset) const {
  static AngleMap map;
  // TODO cache through shared pointers
  static Geometry lastGeometry;   static qreal lastMidTth = 0;
  static QSize    lastSize;       static IJ    lastMid;
  static ImageCut lastImageCut;
  static ImageTransform lastImageTrans;

  qreal midTth = dataset.midTth();
  IJ mid = midPix();
  QSize size   = imageSize();

  if (! (lastMidTth   == midTth    && lastGeometry   == geometry_ &&
         lastSize     == size      && lastMid        == mid       &&
         lastImageCut == imageCut_ && lastImageTrans == imageTransform_)) {
    lastMidTth   = midTth;    lastGeometry = geometry_;
    lastSize     = size;      lastMid      = mid;
    lastImageCut = imageCut_; lastImageTrans = imageTransform_;
    map.calculate(midTth,geometry_,size,imageCut_,mid);
  }

  return map;
}

void Session::setGeometry(qreal detectorDistance, qreal pixSize,
                          bool isMidPixOffset, rcIJ midPixOffset) {
  ASSERT(detectorDistance>0 && pixSize>0)

  geometry_.detectorDistance = detectorDistance;
  geometry_.pixSize          = pixSize;
  geometry_.isMidPixOffset   = isMidPixOffset;
  geometry_.midPixOffset     = midPixOffset;
}

IJ Session::midPix() const {
  auto halfSize = imageSize() / 2;
  IJ mid(halfSize.width(), halfSize.height());

  if (geometry_.isMidPixOffset) {
    rcIJ off = geometry_.midPixOffset;
    mid.i += off.i; mid.j += off.j;
  }

  return mid;
}

shp_Lens Session::lens(bool trans, bool cut, Lens::eNorm norm, rcDataset dataset) const {
  return shp_Lens(new Lens(trans, cut, norm, *this, dataset,
      corrEnabled_ ? corrFile_->datasets().first().data() : nullptr,
      angleMap(dataset), imageCut_, imageTransform_));
}

void Session::setNorm(Lens::eNorm norm) {
  norm_ = norm;
}

qreal Session::calcAvgBackground(rcDataset dataset) const {
  auto l = lens(true, true, Lens::normNONE, dataset);

  auto &map = angleMap(dataset);
  Curve gammaCurve = l->makeCurve(map.rgeGamma(), map.rgeTth());
  auto bgPolynomial = fit::fitPolynomial(bgPolynomialDegree_, gammaCurve, bgRanges_);
  return bgPolynomial.avgY(map.rgeTth());
}

qreal Session::calcAvgBackground(rcDatasets datasets) const {
  qreal bg = 0;

  for (auto &dataset: datasets)
    bg += calcAvgBackground(*dataset);

  return bg / datasets.count();
}

//------------------------------------------------------------------------------
}
// eof
