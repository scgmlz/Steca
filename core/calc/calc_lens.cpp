// ************************************************************************** //
//
//  STeCa2:    StressTextureCalculator ver. 2
//
//! @file      calc_lens.cpp
//!
//! @homepage  http://apps.jcns.fz-juelich.de/steca2
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016
//! @authors   Scientific Computing Group at MLZ Garching
//! @authors   Rebecca Brydon, Jan Burle,  Antti Soininen
//! @authors   Based on the original STeCa by Christian Randau
//
// ************************************************************************** //

#include "calc/calc_lens.h"
#include "session.h"
#include "typ/typ_async.h"
#include <qmath.h>

namespace calc {
//------------------------------------------------------------------------------

using namespace typ;
using namespace data;

LensBase::LensBase(core::Session::rc session, data::Datasets::rc datasets,
                   bool trans, bool cut,
                   typ::ImageTransform::rc imageTransform, typ::ImageCut::rc imageCut)
: session_(session), datasets_(datasets)
, trans_(trans), cut_(cut)
, imageTransform_(imageTransform), imageCut_(imageCut) {
}

LensBase::~LensBase() {
}

size2d LensBase::transCutSize(size2d size) const {
  if (trans_ && imageTransform_.isTransposed())
    size = size.transposed();

  if (cut_)
    size = size - imageCut_.marginSize();

  return size;
}

//------------------------------------------------------------------------------

ImageLens::ImageLens(core::Session::rc session,
                     Image::rc image, Image const* corrImage,
                     Datasets::rc datasets, bool trans, bool cut,
                     ImageCut::rc imageCut, ImageTransform::rc imageTransform)
: super(session, datasets, trans, cut, imageTransform, imageCut)
, image_(image), corrImage_(corrImage)
{
  calcSensCorr();
}

size2d ImageLens::size() const {
  return super::transCutSize(image_.size());
}

inten_t ImageLens::imageInten(uint i, uint j) const {
  if (trans_) doTrans(i, j);
  if (cut_)   doCut(i, j);

  inten_t inten = image_.inten(i, j);
  if (corrImage_)
    inten *= intensCorr_.at(i, j);

  return inten;
}

inten_rge::rc ImageLens::rgeInten(bool fixed) const {
  if (fixed)
    return datasets_.rgeFixedInten(session_, trans_, cut_);

  if (!rgeInten_.isValid()) {
    auto sz = size();
    for_ij (sz.w, sz.h)
      rgeInten_.extendBy(imageInten(i, j));
  }

  return rgeInten_;
}

void ImageLens::doTrans(uint& x, uint& y) const {
  auto s = size();
  uint w = s.w;
  uint h = s.h;

  switch (imageTransform_.val) {
  case ImageTransform::ROTATE_0:
    break;
  case ImageTransform::ROTATE_1:
    qSwap(x, y);
    y = w - y - 1;
    break;
  case ImageTransform::ROTATE_2:
    x = w - x - 1;
    y = h - y - 1;
    break;
  case ImageTransform::ROTATE_3:
    qSwap(x, y);
    x = h - x - 1;
    break;
  case ImageTransform::MIRROR_ROTATE_0:
    x = w - x - 1;
    break;
  case ImageTransform::MIRROR_ROTATE_1:
    y = h - y - 1;
    qSwap(x, y);
    y = w - y - 1;
    break;
  case ImageTransform::MIRROR_ROTATE_2:
    y = h - y - 1;
    break;
  case ImageTransform::MIRROR_ROTATE_3:
    qSwap(x, y);
    break;
  }
}

void ImageLens::calcSensCorr() {
  hasNaNs_ = false;
  if (!corrImage_)
    return;

  ENSURE(image_.size() == corrImage_->size())

  size2d size = corrImage_->size() - imageCut_.marginSize();
  ENSURE(!size.isEmpty())

  qreal sum = 0;

  uint w = size.w, h = size.h, di = imageCut_.left,
       dj = imageCut_.top;

  for_ij (w, h)
    sum += corrImage_->inten(i + di, j + dj);

  qreal avg = sum / (w * h);

  intensCorr_.fill(1, image_.size());

  for_ij (w, h) {
    auto  inten = corrImage_->inten(i + di, j + dj);
    qreal fact;

    if (inten > 0) {
      fact = avg / inten;
    } else {
      fact     = qQNaN();
      hasNaNs_ = true;
    }

    intensCorr_.setAt(i + di, j + dj, inten_t(fact));
  }
}

//------------------------------------------------------------------------------

DatasetLens::DatasetLens(core::Session::rc session,
   data::Dataset::rc dataset, data::Datasets::rc datasets, eNorm norm,
   bool trans, bool cut,
   typ::ImageTransform::rc imageTransform, typ::ImageCut::rc imageCut)
: super(session, datasets, trans, cut, imageTransform, imageCut)
, normFactor_(1), dataset_(dataset)
//>>>: super(session, dataset.image(), corrImage, datasets, trans, cut, imageCut
//, imageTransform), dataset_(dataset), angleMap_(session.angleMap(dataset))
{
  setNorm(norm);
}

//Angles::rc Lens::angles(uint i, uint j) const {
//  if (cut_) doCut(i, j);
//  return angleMap_->at(i, j);
//}

//gma_rge Lens::gmaRangeAt(tth_t tth) const {
//  auto sz = size();
//  uint w = sz.w, h = sz.h;

//  Range rge;

//  for (uint j = 1; j < h; ++j) {
//    for (uint i = 1; i < w; ++i) {
//      auto tthMin = angles(i-1, j).tth, tthMax = angles(i, j).tth;
//      if (tthMin <= tth && tth < tthMax)
//        rge.extendBy(angles(i - 1, j).gma);
//    }
//  }

//  return rge;
//}

Curve DatasetLens::makeCurve(gma_rge::rc rgeGma, tth_rge::rc rgeTth) const {
  uint w = size().w;
  EXPECT(w > 0)

  EXPECT(rgeTth.isValid());
  tth_t minTth = rgeTth.min, deltaTth = rgeTth.width() / w;

  inten_vec intens(w);
  dataset_.collectIntens(session_, intens, rgeGma, minTth, deltaTth);

  Curve res;

  for_i (w)
    res.append(minTth + deltaTth * i, intens.at(i));

  return res;
}

size2d DatasetLens::size() const {
  return super::transCutSize(datasets_.imageSize());
}

gma_rge DatasetLens::rgeGma() const {
  return dataset_.rgeGma(session_);
}

tth_rge DatasetLens::rgeTth() const {
  return dataset_.rgeTth(session_);
}

Curve DatasetLens::makeCurve() const {
  return makeCurve(rgeGma(), rgeTth());
}

Curve DatasetLens::makeAvgCurve() const {
  return datasets_.makeAvgCurve(session_, trans_, cut_);
}

void DatasetLens::setNorm(eNorm norm) {
  auto& datasets = dataset_.datasets();

  qreal num = 1, den = 1;

  switch (norm) {
  case eNorm::DELTA_MONITOR_COUNT:
    num = datasets.avgDeltaMonitorCount();
    den = dataset_.avgDeltaMonitorCount();
    break;
  case eNorm::DELTA_TIME:
    num = datasets.avgDeltaTime();
    den = dataset_.avgDeltaTime();
    break;
  case eNorm::BACKGROUND:
    num = session_.calcAvgBackground(datasets);
    den = session_.calcAvgBackground(dataset_);
    break;
  case eNorm::NONE:
    break;
  }

  normFactor_ = inten_t((num > 0 && den > 0) ? num / den : qQNaN());
  if (qIsNaN(normFactor_))
    MessageLogger::log("Bad normalization value");
}

//------------------------------------------------------------------------------
}
// eof
