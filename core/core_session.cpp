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
  while (0 < numFiles())
    remFile(0);

  remCorrFile();
  corrEnabled_ = false;

  bgPolyDegree_ = 0;
  bgRanges_.clear();

  reflections_.clear();

  norm_ = eNorm::NONE;
}

shp_File Session::file(uint i) const {
  return files_.at(i);
}

bool Session::hasFile(rcstr fileName) {
  QFileInfo fileInfo(fileName);
  for (auto& file : files_)
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
    auto& datasets = file->datasets();

    setImageSize(datasets.imageSize());
    corrImage_ = datasets.folded();  // ensure one dataset

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

void Session::tryEnableCorr(bool on) {
  corrEnabled_ = on && hasCorrFile();
}

void Session::collectDatasetsFromFiles(uint_vec fileNums, uint groupBy) {
  EXPECT(1 <= groupBy)

  collectedFromFiles_ = fileNums;
  collectedDatasets_.clear();
  collectedDatasetsTags_.clear();

  Datasets collectedDatasets;
  for (uint i : collectedFromFiles_) {
    for (auto const& dataset : files_.at(i)->datasets())
      collectedDatasets.appendHere(shp_Dataset(new Dataset(*dataset)));
  }

  if (collectedDatasets.isEmpty()) return;

  Datasets group;
  uint     i = 0;

  auto makeGroup = [this, &group, &groupBy, &i]() {
    if (!group.isEmpty()) {
      str tag = str::number(i + 1);
      i += group.count();

      if (groupBy > 1) tag += '-' + str::number(i);

      collectedDatasets_.appendHere(Dataset::combine(group));
      collectedDatasetsTags_.append(tag);

      group.clear();
    }
  };

  uint gcnt = groupBy;
  for (auto& dataset : collectedDatasets) {
    group.append(dataset);
    if (1 >= gcnt--) {
      makeGroup();
      gcnt = groupBy;
    }
  }

  makeGroup();  // the remaining ones
}

void Session::updateImageSize() {
  if (0 == numFiles() && !hasCorrFile()) imageSize_ = QSize(0, 0);
}

void Session::setImageSize(QSize const& size) THROWS {
  RUNTIME_CHECK(!size.isEmpty(), "bad image size");
  if (imageSize_.isEmpty())
    imageSize_ = size;  // the first one
  else if (imageSize_ != size)
    THROW("inconsistent image size");
}

QSize Session::imageSize() const {
  return imageTransform_.isTransposed() ? imageSize_.transposed() : imageSize_;
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
      if (linked && m1 + m2 >= maxTogether) {
        m1 = m2 = qMax(0, (maxTogether - 1) / 2);
      } else {
        m1 = qMax(qMin(m1, maxTogether - m2 - 1), 0);
        m2 = qMax(qMin(m2, maxTogether - m1 - 1), 0);
      }
    };

    // make sure that cut values are valid; in the right order
    int left = cut.left, top = cut.top, right = cut.right, bottom = cut.bottom;

    if (topLeftFirst) {
      limit(top, bottom, size.height());
      limit(left, right, size.width());
    } else {
      limit(bottom, top, size.height());
      limit(right, left, size.width());
    }

    imageCut_ = ImageCut(left, top, right, bottom);
  }
}

AngleMap const& Session::angleMap(rcDataset dataset) const {
  static AngleMap map;
  // REVIEW cache through shared pointers
  static Geometry       lastGeometry;
  static qreal          lastMidTth = 0;
  static QSize          lastSize;
  static IJ             lastMid;
  static ImageCut       lastImageCut;
  static ImageTransform lastImageTrans;

  qreal midTth = dataset.midTth();
  IJ    mid    = midPix();
  QSize size   = imageSize();

  if (!(lastMidTth == midTth && lastGeometry == geometry_ && lastSize == size &&
        lastMid == mid && lastImageCut == imageCut_ &&
        lastImageTrans == imageTransform_)) {
    lastMidTth     = midTth;
    lastGeometry   = geometry_;
    lastSize       = size;
    lastMid        = mid;
    lastImageCut   = imageCut_;
    lastImageTrans = imageTransform_;
    map.calculate(midTth, geometry_, size, imageCut_, mid);
  }

  return map;
}

void Session::setGeometry(qreal detectorDistance, qreal pixSize,
                          bool isMidPixOffset, rcIJ midPixOffset) {
  EXPECT(detectorDistance > 0 && pixSize > 0)

  geometry_.detectorDistance = detectorDistance;
  geometry_.pixSize          = pixSize;
  geometry_.isMidPixOffset   = isMidPixOffset;
  geometry_.midPixOffset     = midPixOffset;
}

IJ Session::midPix() const {
  auto halfSize = imageSize() / 2;
  IJ   mid(halfSize.width(), halfSize.height());

  if (geometry_.isMidPixOffset) {
    rcIJ off = geometry_.midPixOffset;
    mid.i += off.i;
    mid.j += off.j;
  }

  return mid;
}

shp_ImageLens Session::lens(rcImage image, rcDatasets datasets, bool trans,
                            bool cut) const {
  return shp_ImageLens(
      new ImageLens(*this, image, corrEnabled_ ? &corrImage_ : nullptr,
                    datasets, trans, cut, imageCut_, imageTransform_));
}

shp_Lens Session::lens(rcDataset dataset, rcDatasets datasets, bool trans,
                       bool cut, eNorm norm) const {
  return shp_Lens(new Lens(*this, dataset, corrEnabled_ ? &corrImage_ : nullptr,
                           datasets, trans, cut, norm, angleMap(dataset),
                           imageCut_, imageTransform_));
}

// Calculates the polefigure coordinates alpha and beta with regards to
// sample orientation and diffraction angles.
// tth: Center of reflection's 2theta interval.
// gamma: Center of gamma slice.
void calculateAlphaBeta(rcDataset dataset, deg tth, deg gamma, deg& alpha,
                        deg& beta) {
  // Sample rotations.
  rad omg = dataset.omg().toRad();
  rad phi = dataset.phi().toRad();
  rad chi = dataset.chi().toRad();

  // Rotate a unit vector initially parallel to the y axis with regards to the
  // angles. As a result, the vector is a point on a unit sphere
  // corresponding to the location of a polefigure point.
  // Note that the rotations here do not correspond to C. Randau's dissertation.
  // The rotations given in [J. Appl. Cryst. (2012) 44, 641-644] are incorrect.
  vector3d rotated =
      matrix3d::rotationCWz(phi)
    * matrix3d::rotationCWx(chi)
    * matrix3d::rotationCWz(omg)
    * matrix3d::rotationCWx(gamma.toRad())
    * matrix3d::rotationCCWz(tth.toRad() / 2)
    * vector3d(0, 1, 0);

  // Extract alpha (latitude) and beta (longitude).
  rad alphaRad = acos(rotated._2);
  rad betaRad  = atan2(rotated._0, rotated._1);

  // If alpha is in the wrong hemisphere, mirror both alpha and beta over the
  // center of a unit sphere.
  if (alphaRad >
      M_PI_2) {  // REVIEW - seems like that happens only for a very narrow ring
    alphaRad = qAbs(alphaRad - M_PI);
    betaRad  = betaRad + (betaRad < 0 ? M_PI : -M_PI);
  }

  // Keep beta between 0 and 2pi.
  if (betaRad < 0) betaRad = betaRad + 2 * M_PI;

  alpha = alphaRad.toDeg();
  beta  = betaRad.toDeg();
}

// Fits reflection to the given gamma sector and constructs a ReflectionInfo.
ReflectionInfo Session::makeReflectionInfo(shp_Lens     lens,
                                           rcReflection reflection,
                                           rcRange      gammaSector) const {
  auto curve = lens->makeCurve(gammaSector, lens->angleMap().rgeTth());
  curve =
      curve.subtract(fit::Polynom::fromFit(bgPolyDegree_, curve, bgRanges_));

  QScopedPointer<fit::PeakFunction> peakFunction(
      reflection.peakFunction().clone());

  peakFunction->fit(curve);

  rcRange rgeTth = peakFunction->range();
  deg     alpha, beta;

  rcDataset dataset = lens->dataset();
  calculateAlphaBeta(dataset, rgeTth.center(), gammaSector.center(), alpha,
                     beta);

  XY    peak = peakFunction->fittedPeak();
  qreal fwhm = peakFunction->fittedFWHM();

  shp_Metadata metadata = dataset.metadata();
  ENSURE(metadata)

  return rgeTth.contains(peak.x)
             ? ReflectionInfo(metadata, alpha, beta, gammaSector, peak.y,
                              peak.x, fwhm)
             : ReflectionInfo(metadata, alpha, beta, gammaSector);
}

/* Gathers ReflectionInfos from Datasets.
 * Either uses the whole gamma range of the datasets (if gammaSector is
 * invalid),
 * or user limits the range.
 * Even though the betaStep of the equidistant polefigure grid is needed here,
 * the returned infos won't be on the grid.
 */
ReflectionInfos Session::makeReflectionInfos(rcDatasets   datasets,
                                             rcReflection reflection,
                                             deg betaStep, rcRange gammaRange) {
  ReflectionInfos infos;

  for (auto& dataset : datasets) {
    auto l = lens(*dataset, datasets, true, true, norm_);
    Range rgeGamma = gammaRange.isValid()
                         ? gammaRange
                         : l->gammaRangeAt(reflection.range().center());

    int   numGammaRows = qCeil(rgeGamma.width() / betaStep);
    qreal gammaStep    = rgeGamma.width() / numGammaRows;

    for_i (numGammaRows) {
      qreal min = rgeGamma.min + i * gammaStep;
      Range gammaStripe(min, min + gammaStep);
      auto  refInfo = makeReflectionInfo(l, reflection, gammaStripe);
      if (!qIsNaN(refInfo.inten())) infos.append(refInfo);  // REVIEW
    }
  }

  return infos;
}

void Session::setBgRanges(rcRanges ranges) {
  bgRanges_ = ranges;
}

bool Session::addBgRange(rcRange range) {
  return bgRanges_.add(range);
}

bool Session::remBgRange(rcRange range) {
  return bgRanges_.rem(range);
}

void Session::setBgPolyDegree(uint degree) {
  bgPolyDegree_ = degree;
}

void Session::addReflection(shp_Reflection reflection) {
  reflections_.append(reflection);
}

void Session::remReflection(uint i) {
  reflections_.remove(i);
}

void Session::setNorm(eNorm norm) {
  norm_ = norm;
}

qreal Session::calcAvgBackground(rcDataset dataset) const {
  auto l = lens(dataset, dataset.datasets(), true, true, eNorm::NONE);

  auto& map        = angleMap(dataset);
  Curve gammaCurve = l->makeCurve(map.rgeGamma(), map.rgeTth());

  auto bgPolynom = fit::Polynom::fromFit(bgPolyDegree_, gammaCurve, bgRanges_);
  return bgPolynom.avgY(map.rgeTth());
}

qreal Session::calcAvgBackground(rcDatasets datasets) const {
  qreal bg = 0;

  for (auto& dataset : datasets)
    bg += calcAvgBackground(*dataset);

  return bg / datasets.count();
}

//------------------------------------------------------------------------------
}
// eof
