// ************************************************************************** //
//
//  STeCa2:    StressTexCalculator ver. 2 REVIEW
//
//! @file      core_lens.cpp
//!
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016
//! @authors   Scientific Computing Group at MLZ Garching
//! @authors   Original version: Christian Randau
//! @authors   Version 2: Antti Soininen, Jan Burle, Rebecca Brydon
//
// ************************************************************************** //

#include "core_lens.h"

#include "core_dataset.h"
#include "core_session.h"
#include "types/core_type_curve.h"
#include "types/core_type_geometry.h"
#include "types/core_type_image.h"
#include "types/core_type_image_transform.h"
#include <QStringList>
#include <qmath.h>

namespace core {
//------------------------------------------------------------------------------

ImageLens::ImageLens(rcSession session, rcImage image, Image const* corrImage,
           rcDatasets datasets, bool trans, bool cut, ImageCut const& imageCut, ImageTransform const& imageTransform)
: session_(session), image_(image), corrImage_(corrImage)
, datasets_(datasets) , trans_(trans), cut_(cut), imageTransform_(imageTransform), imageCut_(imageCut)
, normFactor_(1)
{
  calcSensCorr();
}

QSize ImageLens::size() const {
  QSize size = image_.size();

  if (trans_ && imageTransform_.isTransposed())
    size.transpose();

  if (cut_)
    size -= imageCut_.marginSize();

  return size;
}

inten_t ImageLens::inten(uint i, uint j) const {
  if (trans_) doTrans(i,j);
  if (cut_)   doCut(i,j);

  inten_t inten = image_.inten(i,j);
  if (corrImage_) inten *= intensCorr_.at(i,j);

  return inten * normFactor_;
}

rcRange ImageLens::rgeInten(bool fixed) const {
  if (fixed)
    return datasets_.rgeFixedInten(session_,trans_,cut_);

  if (!rgeInten_.isValid()) {
    auto sz = size();
    for_ij (sz.width(), sz.height())
      rgeInten_.extendBy(inten(i,j));
  }

  return rgeInten_;
}

void ImageLens::doTrans(uint& x, uint& y) const {
  auto s = size();
  uint w = s.width();
  uint h = s.height();

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
  default:
    NEVER_HERE;
  }
}

void ImageLens::doCut(uint& i, uint& j) const {
  i += imageCut_.left; j += imageCut_.top;
}

void ImageLens::calcSensCorr() {
  hasNaNs_ = false;
  if (!corrImage_) return;

  ENSURE(image_.size() == corrImage_->size())

  QSize size = corrImage_->size();
  size -= imageCut_.marginSize();
  ENSURE(!size.isEmpty())

  qreal sum = 0;

  uint w = size.width(), h = size.height(),
      di = imageCut_.left, dj = imageCut_.top;

  for_ij (w,h)
    sum += corrImage_->inten(i+di, j+dj);

  qreal avg = sum / (w * h);

  intensCorr_.fill(1,image_.size());

  for_ij (w,h) {
    auto inten = corrImage_->inten(i+di,j+dj);
    qreal fact;

    if (inten > 0) {
      fact = avg / inten;
    } else {
      fact = qQNaN(); hasNaNs_ = true;
    }

    intensCorr_.setAt(i+di,j+dj, fact);
  }
}

//------------------------------------------------------------------------------

str_lst const& Lens::normStrLst() {
  static str_lst strLst {
    "Disable", "Delta time", "Monitor count", "Background level",
  };

  return strLst;
}

Lens::Lens(rcSession session, rcDataset dataset, Image const* corrImage,
                         rcDatasets datasets, bool trans, bool cut, eNorm norm,
                         AngleMap const& angleMap, ImageCut const& imageCut, ImageTransform const& imageTransform)
: super(session, dataset.image(), corrImage, datasets, trans, cut, imageCut, imageTransform)
, dataset_(dataset), angleMap_(angleMap)
{
  setNorm(norm);
}

Angles const& Lens::angles(uint i, uint j) const {
  if (cut_) doCut(i,j);
  return angleMap_.at(i,j);
}

Range Lens::gammaRangeAt(qreal tth) const {
  auto sz = size(); uint w = sz.width(), h = sz.height();

  Range rge;

  for (uint j=1; j<h; ++j) {
    for (uint i=1; i<w; ++i) {
      auto tthMin = angles(i-1,j).tth, tthMax = angles(i,j).tth; // TODO optimize?
      if (tthMin <= tth && tth < tthMax)
        rge.extendBy(angles(i-1,j).gamma);
    }
  }

  return rge;
}

Curve Lens::makeCurve(rcRange gammaRange, rcRange tthRange) const {
  auto s = size();
  uint w = s.width(), h = s.height();

  qreal const deltaTTH = tthRange.width() / w;

  qreal_vec intens_vec(w);
  uint_vec  counts_vec(w, 0);

  for_ij (w, h) {
    // TODO angles can be arranged for a single loop for_i (pixTotal)
    // [last in commit 98413db71cd38ebaa54b6337a6c6e670483912ef]
    auto const& as = angles(i,j);
    if (!gammaRange.contains(as.gamma)) continue;

    int bin = (as.tth == tthRange.max)
              ? w - 1
              : qFloor((as.tth - tthRange.min) / deltaTTH);

    if (bin < 0 || (int)w <= bin) {
      //        TR("TTH bin outside cut?")
      continue; // outside of the cut
    }

    auto in = inten(i,j);
    if (!qIsNaN(in)) {
      intens_vec[bin] += in;
      counts_vec[bin] += 1;
    }
  }

  Curve res;

  for_i (w) {
    auto in  = intens_vec[i];
    auto cnt = counts_vec[i];
    if (cnt > 0) in /= cnt;
    res.append(tthRange.min + deltaTTH * i, in);
  }

  return res;
}

Curve Lens::makeAvgCurve() const {
  return datasets_.makeAvgCurve(session_, trans_, cut_);
}

void Lens::setNorm(eNorm norm) {
  auto &datasets = dataset_.datasets();

  switch (norm) {
  case eNorm::NONE:
    normFactor_ = 1;
    break;
  case eNorm::DELTA_MONITOR_COUNT:
    // could be NaN (divide by 0)
    normFactor_ = datasets.avgDeltaMonitorCount() / dataset_.deltaMonitorCount();
    break;
  case eNorm::DELTA_TIME:
    // could be NaN (divide by 0)
    normFactor_ = datasets.avgDeltaTime() / dataset_.deltaTime();
    break;
  case eNorm::BACKGROUND:
    // could be NaN (divide by 0)
    normFactor_ = session_.calcAvgBackground(datasets) /
                  session_.calcAvgBackground(dataset_);
    break;
  }
}

//------------------------------------------------------------------------------
}
// eof
