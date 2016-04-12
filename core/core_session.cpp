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
#include "io/core_io.h"
#include "core_fit_fitting.h"
#include "types/core_type_curve.h"

namespace core {
//------------------------------------------------------------------------------

Session::Session() {
  clear();
}

void Session::clear() {
  while (numFiles(false)) remFile(0);

  remCorrFile();
  _corrEnabled = false;

  _bgPolynomialDegree = 0;
  _bgRanges.clear();

  _reflections.clear();

  _norm = Lens::normNONE;
}

uint Session::numFiles(bool withCorr) const {
  return _files.count() + (withCorr && !_corrFile.isNull() ? 1 : 0);
}

shp_File Session::file(uint i) const {
  if ((uint)_files.count() == i) {
    // past the vector end, must be the correction file
    ASSERT(!_corrFile.isNull())
    return _corrFile;
  } else {
    return _files.at(i);
  }
}

shp_File Session::addFile(rcstr fileName) THROWS {
  if (fileName.isEmpty() || hasFile(fileName))
    return shp_File();  // no new file

  shp_File file = io::load(fileName);
  setImageSize(file->datasets().imageSize());

  // all ok
  _files.append(file);
  return file;
}

shp_File Session::remFile(uint i) {
  shp_File file = _files.at(i);
  _files.remove(i);
  updateImageSize();
  return file;
}

bool Session::hasFile(rcstr fileName) {
  QFileInfo fileInfo(fileName);
  for (auto &file: _files)
    if (fileInfo == file->fileInfo()) return true;
  return false;
}

shp_File Session::setCorrFile(rcstr fileName) {
  if (fileName.isEmpty()) {
    remCorrFile();
    return shp_File();
  }

  shp_File file = io::load(fileName);
  file->datasets().fold(); // ensure one dataset

  setImageSize(file->datasets().imageSize());

  // all ok
  _corrFile    = file;
  _corrEnabled = true;

  return file;
}

void Session::remCorrFile() {
  _corrFile.clear();
  _corrEnabled = false;
  updateImageSize();
}

void Session::enableCorr(bool on) {
  _corrEnabled = on && hasCorrFile();
}

void Session::updateImageSize() {
  if (0 == numFiles(true))
    _imageSize = QSize(0,0);
}

void Session::setImageSize(QSize const& size) THROWS {
  RUNTIME_CHECK (!size.isEmpty(), "bad image size");
  if (_imageSize.isEmpty())
    _imageSize = size; // the first one
  else if (_imageSize != size)
    THROW("inconsistent image size");
}

void Session::setImageTransformMirror(bool on) {
  _imageTransform = _imageTransform.mirror(on);
}

void Session::setImageTransformRotate(ImageTransform const& rot) {
  _imageTransform = _imageTransform.rotateTo(rot);
}

void Session::setImageCut(bool topLeftFirst, bool linked, ImageCut const& cut) {
  auto size = _imageSize;

  if (size.isEmpty())
    _imageCut = ImageCut();
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

    _imageCut = ImageCut(left,top,right,bottom);
  }
}

AngleMap const& Session::angleMap(rcDataset dataset) const {
  static AngleMap map;

  // TODO cache through shared pointers
  static qreal midTth_ = 0;
  static Geometry geometry_;
  static QSize size_; static IJ mid_;

  qreal midTth = dataset.midTth();
  QSize size   = dataset.imageSize();
  IJ    mid    = midPix();

  if (! (midTth_ == midTth && geometry_ == _geometry &&
         size_ == size && mid_ == mid)) {
    midTth_ = midTth; geometry_ = _geometry; size_ = size; mid_ = mid;
    map.calculate(midTth,_geometry,size,_imageCut,mid);
  }

  return map;
}

void Session::setGeometry(qreal detectorDistance, qreal pixSize,
                          bool isMidPixOffset, rcIJ midPixOffset) {
  ASSERT(detectorDistance>0 && pixSize>0)

  _geometry.detectorDistance = detectorDistance;
  _geometry.pixSize          = pixSize;
  _geometry.isMidPixOffset   = isMidPixOffset;
  _geometry.midPixOffset     = midPixOffset;
}

IJ Session::midPix() const {
  auto halfSize = _imageSize / 2;
  IJ mid(halfSize.width(), halfSize.height());

  if (_geometry.isMidPixOffset) {
    rcIJ off = _geometry.midPixOffset;
    mid.i += off.i; mid.j += off.j;
  }

  return mid;
}

shp_Lens Session::lens(bool trans, bool cut, Lens::eNorm norm, rcDataset dataset) const {
  return shp_Lens(new Lens(trans, cut, norm, *this, dataset,
      _corrEnabled ? _corrFile->datasets().first().data() : nullptr,
      angleMap(dataset), _imageCut, _imageTransform));
}

void Session::setNorm(Lens::eNorm norm) {
  _norm = norm;
}

qreal Session::calcAvgBackground(rcDataset dataset) const {
  auto l = lens(true, true, Lens::normNONE, dataset);

  auto &map = angleMap(dataset);
  Curve gammaCurve = l->makeCurve(map.rgeGamma(), map.rgeTth());
  auto bgPolynomial = fit::fitPolynomial(_bgPolynomialDegree, gammaCurve, _bgRanges);
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
