/*******************************************************************************
 * REVIEW: STeCa2 - StressTextureCalculator ver. 2
 *
 * Copyright (C) 2016 Forschungszentrum Jülich GmbH 2016
 *
 * This program is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the Free
 * Software Foundation, either version 3 of the License, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE.
 *
 * See the COPYING and AUTHORS files for more details.
 ******************************************************************************/

#include "session.h"
#include "lib/typ/typ_matrix.h"
#include "fit/fit_fun.h"
#include <qmath.h>

namespace core {
//------------------------------------------------------------------------------

using namespace typ;
using namespace data;
using namespace calc;

Session::Session() : angleMapCache_(360) {
  clear();
  fit::initFactory();
}

void Session::clear() {
  while (0 < numFiles())
    remFile(0);

  remCorrFile();
  corrEnabled_ = corrHasNaNs_ = false;

  bgPolyDegree_ = 0;
  bgRanges_.clear();

  reflections_.clear();

  norm_ = eNorm::NONE;

  angleMapCache_.clear();
}

shp_File Session::file(uint i) const {
  return files_.at(i);
}

bool Session::hasFile(rcstr fileName) {
  QFileInfo fileInfo(fileName);
  for (auto& file : files_)
    if (fileInfo == file->fileInfo())
      return true;
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

void Session::calcIntensCorr() const {
  corrHasNaNs_ = false;

  size2d size = corrImage_.size() - imageCut_.marginSize();
  ENSURE(!size.isEmpty())

  uint w = size.w, h = size.h, di = imageCut_.left,
       dj = imageCut_.top;

  qreal sum = 0;
  for_ij (w, h)
    sum += corrImage_.inten(i + di, j + dj);

  qreal avg = sum / (w * h);

  intensCorr_.fill(1, corrImage_.size());

  for_ij (w, h) {
    auto  inten = corrImage_.inten(i + di, j + dj);
    qreal fact;

    if (inten > 0) {
      fact = avg / inten;
    } else {
      fact = qQNaN();
      corrHasNaNs_ = true;
    }

    intensCorr_.setInten(i + di, j + dj, inten_t(fact));
  }
}

Image const* Session::intensCorr() const {
  if (!isCorrEnabled())
    return nullptr;

  if (intensCorr_.isEmpty())
    calcIntensCorr();

  return &intensCorr_;
}

void Session::setCorrFile(shp_File file) THROWS {
  if (file.isNull()) {
    remCorrFile();
  } else {
    auto& datasets = file->datasets();

    setImageSize(datasets.imageSize());
    corrImage_ = datasets.foldedImage();
    intensCorr_.clear();  // will be calculated lazily

    // all ok
    corrFile_    = file;
    corrEnabled_ = true;
  }
}

void Session::remCorrFile() {
  corrFile_.clear();
  corrImage_.clear(); intensCorr_.clear();
  corrEnabled_ = false;
  updateImageSize();
}

void Session::tryEnableCorr(bool on) {
  corrEnabled_ = on && hasCorrFile();
}

void Session::collectDatasetsFromFiles(uint_vec fileNums, pint combineBy) {

  collectedFromFiles_ = fileNums;
  collectedDatasets_.clear();
  collectedDatasetsTags_.clear();

  vec<shp_OneDataset> datasetsFromFiles;
  for (uint i : collectedFromFiles_)
    for (auto& dataset : files_.at(i)->datasets())
      datasetsFromFiles.append(dataset);

  if (datasetsFromFiles.isEmpty())
    return;

  shp_Dataset cd(new Dataset);
  uint i = 0;

  auto appendCd = [this, &cd, &combineBy, &i]() {
    uint cnt = cd->count();
    if (cnt) {
      str tag = str::number(i + 1);
      i += cnt;

      if (combineBy > 1)
        tag += '-' + str::number(i);

      collectedDatasets_.appendHere(cd);
      collectedDatasetsTags_.append(tag);

      cd = shp_Dataset(new Dataset);
    }
  };

  uint by = combineBy;
  for (auto& dataset : datasetsFromFiles) {
    cd->append(shp_OneDataset(dataset));
    if (1 >= by--) {
      appendCd();
      by = combineBy;
    }
  }

  appendCd();  // the remaining ones
}

void Session::updateImageSize() {
  if (0 == numFiles() && !hasCorrFile())
    imageSize_ = size2d(0, 0);
}

void Session::setImageSize(size2d::rc size) THROWS {
  RUNTIME_CHECK(!size.isEmpty(), "bad image size");
  if (imageSize_.isEmpty())
    imageSize_ = size;  // the first one
  else if (imageSize_ != size)
    THROW("inconsistent image size");
}

size2d Session::imageSize() const {
  return imageTransform_.isTransposed() ? imageSize_.transposed() : imageSize_;
}

void Session::setImageTransformMirror(bool on) {
  imageTransform_ = imageTransform_.mirror(on);
}

void Session::setImageTransformRotate(ImageTransform::rc rot) {
  imageTransform_ = imageTransform_.rotateTo(rot);
}

void Session::setImageCut(bool topLeftFirst, bool linked, ImageCut::rc cut) {
  auto size = imageSize_;

  if (size.isEmpty())
    imageCut_ = ImageCut();
  else {
    auto limit = [linked](uint& m1, uint& m2, uint maxTogether) {
      if (linked && m1 + m2 >= maxTogether) {
        m1 = m2 = qMax((maxTogether - 1) / 2, 0u);
      } else {
        m1 = qMax(qMin(m1, maxTogether - m2 - 1), 0u);
        m2 = qMax(qMin(m2, maxTogether - m1 - 1), 0u);
      }
    };

    // make sure that cut values are valid; in the right order
    uint left = cut.left, top = cut.top, right = cut.right, bottom = cut.bottom;

    if (topLeftFirst) {
      limit(top, bottom, size.h);
      limit(left, right, size.w);
    } else {
      limit(bottom, top, size.h);
      limit(right, left, size.w);
    }

    imageCut_ = ImageCut(left, top, right, bottom);
  }

  intensCorr_.clear();  // lazy
}

void Session::setGeometry(preal detectorDistance, preal pixSize, IJ::rc midPixOffset) {

  geometry_.detectorDistance = detectorDistance;
  geometry_.pixSize          = pixSize;
  geometry_.midPixOffset     = midPixOffset;
}

IJ Session::midPix() const {
  auto halfSize = imageSize().scaled(.5);
  IJ mid(to_i(halfSize.w), to_i(halfSize.h));

  IJ::rc off = geometry_.midPixOffset;
  mid.i += off.i;
  mid.j += off.j;

  return mid;
}

Range::rc Session::gammaRange() const {
  return gammaRange_;
}

void Session::setGammaRange(Range::rc gammaRange) {
  gammaRange_ = gammaRange;
}

shp_AngleMap Session::angleMap(OneDataset::rc one) const {
  AngleMap::Key key(geometry_, imageSize_, imageCut_, midPix(), one.midTth());
  shp_AngleMap map = angleMapCache_.value(key);
  if (map.isNull())
    map = angleMapCache_.insert(key, shp_AngleMap(new AngleMap(key)));
  return map;
}

shp_AngleMap Session::angleMap(Session::rc session, OneDataset::rc one) {
  return session.angleMap(one);
}

shp_ImageLens Session::imageLens(
    Image::rc image, Datasets::rc datasets, bool trans, bool cut) const
{
  return shp_ImageLens(new ImageLens(*this, image, datasets, trans, cut));
}

shp_DatasetLens Session::datasetLens(Dataset::rc dataset, Datasets::rc datasets, eNorm norm,
                                     bool trans, bool cut) const {
  return shp_DatasetLens(new DatasetLens(*this, dataset, datasets, norm,
                         trans, cut, imageTransform_, imageCut_));
}

// Calculates the polefigure coordinates alpha and beta with regards to
// sample orientation and diffraction angles.
// tth: Center of reflection's 2theta interval.
// gma: Center of gamma slice.
void calculateAlphaBeta(Dataset::rc dataset, tth_t tth, gma_t gma,
                        deg& alpha, deg& beta) {
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
    * matrix3d::rotationCWx(gma.toRad())
    * matrix3d::rotationCCWz(tth.toRad() / 2)
    * vector3d(0, 1, 0);

  // Extract alpha (latitude) and beta (longitude).
  rad alphaRad = acos(rotated._2);
  rad betaRad  = atan2(rotated._0, rotated._1);

  // If alpha is in the wrong hemisphere, mirror both alpha and beta over the
  // center of a unit sphere.
  if (alphaRad > M_PI_2) {  // REVIEW - seems like that happens only for a very narrow ring
    alphaRad = qAbs(alphaRad - M_PI);
    betaRad  = betaRad + (betaRad < 0 ? M_PI : -M_PI);
  }

  // Keep beta between 0 and 2pi.
  if (betaRad < 0) betaRad = betaRad + 2 * M_PI;

  alpha = alphaRad.toDeg();
  beta  = betaRad.toDeg();
}

Curve Session::makeCurve(DatasetLens::rc lens, gma_rge::rc rgeGma, bool averaged) const {
  Curve curve = lens.makeCurve(rgeGma, averaged);
  curve.subtract(fit::Polynom::fromFit(bgPolyDegree_, curve, bgRanges_));

  return curve;
}

// Fits reflection to the given gamma sector and constructs a ReflectionInfo.
ReflectionInfo Session::makeReflectionInfo(
    DatasetLens::rc lens, Reflection::rc reflection, gma_rge::rc gmaSector,
    bool averaged) const {
  Curve curve = makeCurve(lens, gmaSector, averaged);

  scoped<fit::PeakFunction*> peakFunction(reflection.peakFunction().clone());

  peakFunction->fit(curve);

  Range::rc rgeTth = peakFunction->range();
  deg     alpha, beta;

  Dataset::rc dataset = lens.dataset();
  calculateAlphaBeta(dataset, rgeTth.center(), gmaSector.center(), alpha,
                     beta);

  peak_t peak      = peakFunction->fittedPeak();
  fwhm_t fwhm      = peakFunction->fittedFWHM();
  peak_t peakError = peakFunction->peakError();
  fwhm_t fwhmError = peakFunction->fwhmError();

  shp_Metadata metadata = dataset.metadata();

  return rgeTth.contains(peak.x)
             ? ReflectionInfo(metadata, alpha, beta, gmaSector,
                              inten_t(peak.y), inten_t(peakError.y),
                              tth_t(peak.x),   tth_t(peakError.x),
                              fwhm_t(fwhm),    fwhm_t(fwhmError))
             : ReflectionInfo(metadata, alpha, beta, gmaSector);
}

/* Gathers ReflectionInfos from Datasets.
 * Either uses the whole gamma range of the datasets (if gammaSector is
 * invalid), or user limits the range.
 * Even though the betaStep of the equidistant polefigure grid is needed here,
 * the returned infos won't be on the grid. REVIEW gammaStep separately?
 */
ReflectionInfos Session::makeReflectionInfos(
    Datasets::rc datasets, Reflection::rc reflection,
    uint gmaSlices, gma_rge::rc rgeGma, bool averaged, Progress* progress)
{
  ReflectionInfos infos;

  if (progress)
    progress->setTotal(datasets.count());

  for (auto& dataset : datasets) {
    if (progress)
      progress->step();

    auto lens = datasetLens(*dataset, datasets, norm_, true, true);

    Range rge = (gmaSlices > 0) ? lens->rgeGma() : lens->rgeGmaFull();
    if (rgeGma.isValid())
      rge = rge.intersect(rgeGma);

    if (rge.isEmpty())
      continue;

    gmaSlices = qMax(1u, gmaSlices);
    qreal step = rge.width() / gmaSlices;
    for_i (uint(gmaSlices)) {
      qreal min = rge.min + i * step;
      gma_rge gmaStripe(min, min + step);
      auto refInfo = makeReflectionInfo(*lens, reflection, gmaStripe, averaged);
      if (!qIsNaN(refInfo.inten()))
        infos.append(refInfo);
    }
  }

  return infos;
}

void Session::setBgRanges(Ranges::rc ranges) {
  bgRanges_ = ranges;
}

bool Session::addBgRange(Range::rc range) {
  return bgRanges_.add(range);
}

bool Session::remBgRange(Range::rc range) {
  return bgRanges_.rem(range);
}

void Session::setBgPolyDegree(uint degree) {
  bgPolyDegree_ = degree;
}

void Session::addReflection(shp_Reflection reflection) {
  EXPECT(!reflection.isNull())
  reflections_.append(reflection);
}

void Session::remReflection(uint i) {
  reflections_.remove(i);
}

void Session::setNorm(eNorm norm) {
  norm_ = norm;
}

qreal Session::calcAvgBackground(Dataset::rc dataset) const {
  auto lens = datasetLens(dataset, dataset.datasets(), eNorm::NONE, true, true);

  Curve gmaCurve = lens->makeCurve(true); // REVIEW averaged?
  auto bgPolynom = fit::Polynom::fromFit(bgPolyDegree_, gmaCurve, bgRanges_);
  return bgPolynom.avgY(lens->rgeTth());
}

qreal Session::calcAvgBackground(Datasets::rc datasets) const {
  TakesLongTime __;

  qreal bg = 0;

  for (auto& dataset : datasets)
    bg += calcAvgBackground(*dataset);

  return bg / datasets.count();
}

//------------------------------------------------------------------------------
}
// eof
