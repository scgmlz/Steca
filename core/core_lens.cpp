// ************************************************************************** //
//
//  STeCa2:    StressTexCalculator ver. 2
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

str_lst const& Lens::normStrLst() {
  static str_lst strLst {
    "Disable", "Delta time", "Monitor count", "Background level",
  };

  return strLst;
}

Lens::Lens(bool trans, bool cut, eNorm norm, rcSession session,
           rcDataset dataset, Dataset const* corrDataset,
           AngleMap const& angleMap, ImageCut const& imageCut, ImageTransform const& imageTransform)
: trans_(trans), cut_(cut), session_(session)
, dataset_(dataset), corrDataset_(corrDataset)
, angleMap_(angleMap), imageCut_(imageCut), imageTransform_(imageTransform)
{
  calcSensCorr();
  setNorm(norm);
}

QSize Lens::size() const {
  QSize size = dataset_.imageSize();

  if (trans_)
    if (imageTransform_.isTransposed())
      size.transpose();

  if (cut_)
    size -= imageCut_.marginSize();


  return size;
}

inten_t Lens::inten(uint i, uint j) const {
  if (trans_)
    doTrans(i,j);
  if (cut_)
    doCut(i,j);

  inten_t inten = dataset_.inten(i,j);

  if (corrDataset_)
    inten *= intensCorr_.at(i,j);

  return inten * normFactor_;
}

Angles const& Lens::angles(uint i, uint j) const {
  if (trans_)
    doTrans(i,j);
  if (cut_)
    doCut(i,j);

  return angleMap_.at(i,j);
}

rcRange Lens::rgeInten() const {
  if (!rgeInten_.isValid()) {
    auto s = size();
    for_ij (s.width(), s.height())
      rgeInten_.extendBy(inten(i,j));
  }

  return rgeInten_;
}

rcRange Lens::rgeIntenGlobal() const {
  if (!rgeIntenGlobal_.isValid()) {
    for (auto const& dataset: dataset_.datasets()) {
      // a copy of this lens for each dataset
      Lens lens(trans_, cut_, session_.norm(), session_,
                *dataset, corrDataset_,
                angleMap_,imageCut_,imageTransform_);
      rgeIntenGlobal_.extendBy(lens.rgeInten());
    }
  }

  return rgeIntenGlobal_;
}

Curve Lens::makeCurve(rcRange gammaRange, rcRange tthRange) const {

  Curve res;

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

  for_i (w) {
    auto in  = intens_vec[i];
    auto cnt = counts_vec[i];
    if (cnt > 0) in /= cnt;
    res.append(tthRange.min + deltaTTH * i, in);
  }

  return res;
}

void Lens::doTrans(uint& x, uint& y) const {
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

void Lens::doCut(uint& i, uint& j) const {
  i += imageCut_.left; j += imageCut_.top;
}

void Lens::calcSensCorr() {
  hasNaNs_ = false;
  if (!corrDataset_) return;

  ASSERT(dataset_.imageSize() == corrDataset_->imageSize())

  QSize size = corrDataset_->imageSize();
  size -= imageCut_.marginSize();
  ASSERT(!size.isEmpty())

  qreal sum = 0;

  uint w = size.width(), h = size.height(),
      di = imageCut_.left, dj = imageCut_.top;

  for_ij(w,h)
    sum += corrDataset_->inten(i+di, j+dj);

  qreal avg = sum / (w * h);

  intensCorr_.fill(size);

  for_ij(w,h) {
    auto inten = corrDataset_->inten(i+di,j+dj);
    qreal fact;

    if (inten > 0) {
      fact = avg / inten;
    } else {
      fact = qQNaN(); hasNaNs_ = true;
    }

    intensCorr_.setAt(i,j, fact);
  }
}

void Lens::setNorm(eNorm norm) {
  auto &datasets = dataset_.datasets();

  switch (norm) {
  case normNONE:
    normFactor_ = 1;
    break;
  case normDELTA_MONITOR_COUNT:
    // could be NaN (divide by 0)
    normFactor_ = datasets.avgDeltaMonitorCount() / dataset_.deltaMonitorCount();
    break;
  case normDELTA_TIME:
    // could be NaN (divide by 0)
    normFactor_ = datasets.avgDeltaTime() / dataset_.deltaTime();
    break;
  case normBACKGROUND:
    // could be NaN (divide by 0)
    normFactor_ = session_.calcAvgBackground(datasets) /
                  session_.calcAvgBackground(dataset_);
    break;
  }
}

}

// eof
