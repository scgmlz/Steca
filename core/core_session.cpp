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
#include "core_lens.h"
#include "core_reflection.h"
#include "core_reflection_info.h"
#include "types/core_type_curve.h"
#include "types/core_type_matrix.h"
#include <qmath.h>

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

  norm_ = eNorm::NONE;
}

shp_File Session::file(uint i) const {
  return files_.at(i);
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

    setImageSize(datasets.imageSize());
    corrImage_ = datasets.folded(); // ensure one dataset

    // all ok
    corrFile_    = file;
    corrEnabled_ = true;
  }
}

void Session::remCorrFile() {
  corrFile_.clear();
  corrImage_.clear();
  corrEnabled_ = false;
  updateImageSize();
}

void Session::enableCorr(bool on) {
  corrEnabled_ = on && hasCorrFile();
}

void Session::collectDatasetsFromFiles(uint_vec fileNums) {
  collectedFromFiles_ = fileNums;

  collectedDatasets_.clear();
  for (uint i: collectedFromFiles_) {
    for (auto &ds: files_.at(i)->datasets()) {
      collectedDatasets_.appendHere(ds);
    }
  }
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

shp_ImageLens Session::lens(rcImage image, bool trans, bool cut) const {
  return shp_ImageLens(new ImageLens(*this, image, corrEnabled_ ? &corrImage_ : nullptr,
                                     trans, cut, imageCut_, imageTransform_));
}

shp_Lens Session::lens(rcDataset dataset, bool trans, bool cut, eNorm norm) const {
  return shp_Lens(new Lens(*this, dataset, corrEnabled_ ? &corrImage_ : nullptr,
                           trans, cut, norm, angleMap(dataset),
                           imageCut_, imageTransform_));
}

// Calculates the polefigure coordinates alpha and beta with regards to
// sample orientation and diffraction angles.
  void calculateAlphaBeta(rcDataset dataset,
                               qreal tth, qreal gamma,
                               qreal& alpha, qreal& beta) {
  // Sample rotations.
  qreal omg = deg2rad(dataset.omg());
  qreal phi = deg2rad(dataset.phi());
  qreal chi = deg2rad(dataset.chi());

  // Center of reflection's 2theta interval.
  tth   = deg2rad(tth);
  // Center of gamma slice.
  gamma = deg2rad(gamma);

  // Rotate a unit vector initially parallel to the y axis with regards to the
  // angles. As a result, the vector is a point on a unit sphere
  // corresponding to the location of a polefigure point.
  // Note that the rotations here do not correspond to C. Randau's dissertation.
  // The rotations given in [J. Appl. Cryst. (2012) 44, 641-644] are incorrect.
  vector3d rotated = matrix3d::rotationCWz (phi) // TODO precalculate phi/ch/omg rotations, save in dataset
                   * matrix3d::rotationCWx (chi)
                   * matrix3d::rotationCWz (omg)
                   * matrix3d::rotationCWx (gamma)
                   * matrix3d::rotationCCWz(tth / 2)
                   * vector3d(0,1,0);

  // Extract alpha (latitude) and beta (longitude).
  alpha = acos(rotated._2);
  beta  = atan2(rotated._0, rotated._1);

  // If alpha is in the wrong hemisphere, mirror both alpha and beta over the
  // center of a unit sphere.
  if (alpha > M_PI_2) { // REVIEW - seems like that happens only for a very narrow ring
//    alpha = qAbs(alpha - M_PI);
//    beta += beta < 0 ? M_PI : -M_PI;
  }

  // Keep beta between 0 and 2pi.
  if (beta < 0)
    beta += 2 * M_PI;

  alpha = rad2deg(alpha);
  beta  = rad2deg(beta);
}

// Fits reflection to the given gamma sector and constructs a ReflectionInfo.
ReflectionInfo Session::makeReflectionInfo(shp_Lens lens, rcReflection reflection,
                                           rcRange gammaSector) const {
  auto curve = lens->makeCurve(gammaSector, lens->angleMap().rgeTth());
  auto bgPol = fit::fitPolynomial(bgPolynomialDegree_,curve,bgRanges_);
  curve = curve.subtract(bgPol);

  QScopedPointer<fit::PeakFunction> peakFunction(reflection.peakFunction().clone());

  rcRange rgeTth = reflection.range();
  fit::fit(*peakFunction, curve, rgeTth);

  qreal alpha, beta;
  calculateAlphaBeta(lens->dataset(), rgeTth.center(), gammaSector.center(), alpha, beta);

  XY peak = peakFunction->fittedPeak();
  return ReflectionInfo(alpha, beta, gammaSector,
                        peak.y,peak.x,peakFunction->fittedFWHM());
}

/* Gathers ReflectionInfos from Datasets.
 * Either uses the whole gamma range of the datasets (if gammaSector is invalid),
 * or user limits the range.
 * Even though the betaStep of the equidistant polefigure grid is needed here,
 * the returned infos won't be on the grid.
 */
ReflectionInfos Session::reflectionInfos(rcDatasets datasets, rcReflection reflection,
                                         qreal betaStep, rcRange gammaRange) {
  ReflectionInfos infos;

  for (auto &dataset: datasets) {
    auto l = lens(*dataset, true, true, norm_);
    // TODO potentially optimize (invariant if lens does not change)
    Range rgeGamma = gammaRange.isValid()
        ? gammaRange
        : l->gammaRangeAt(reflection.range().center());

    int numGammaRows = qCeil(rgeGamma.width() / betaStep);
    qreal gammaStep = rgeGamma.width() / numGammaRows;

    for_i (numGammaRows) {
      qreal min = rgeGamma.min + i * gammaStep;
      Range gammaStripe(min,min + gammaStep);
      infos.append(makeReflectionInfo(l,reflection,gammaStripe));
    }
  }

  return infos;
}

void Session::setNorm(eNorm norm) {
  norm_ = norm;
}

qreal Session::calcAvgBackground(rcDataset dataset) const {
  auto l = lens(dataset, true, true, eNorm::NONE);

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
