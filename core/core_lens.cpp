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
: _trans(trans), _cut(cut), _session(session)
, _dataset(dataset), _corrDataset(corrDataset)
, _angleMap(angleMap), _imageCut(imageCut), _imageTransform(imageTransform)
{
  calcSensCorr();
  setNorm(norm);
}

QSize Lens::size() const {
  QSize size = _dataset.imageSize();

  if (_trans)
    if (_imageTransform.isTransposed())
      size.transpose();

  if (_cut)
    size -= _imageCut.marginSize();


  return size;
}

inten_t Lens::inten(uint i, uint j) const {
  if (_trans)
    doTrans(i,j);
  if (_cut)
    doCut(i,j);

  inten_t inten = _dataset.inten(i,j);

  if (_corrDataset)
    inten *= _intensCorr.at(i,j);

  return inten * _normFactor;
}

Angles const& Lens::angles(uint i, uint j) const {
  if (_trans)
    doTrans(i,j);
  if (_cut)
    doCut(i,j);

  return _angleMap.at(i,j);
}

rcRange Lens::rgeInten() const {
  if (!_rgeInten.isValid()) {
    auto s = size();
    for_ij (s.width(), s.height())
      _rgeInten.extendBy(inten(i,j));
  }

  return _rgeInten;
}

rcRange Lens::rgeIntenGlobal() const {
  if (!_rgeIntenGlobal.isValid()) {
    for (auto const& dataset: _dataset.datasets()) {
      // a copy of this lens for each dataset
      Lens lens(_trans, _cut, _session.norm(), _session,
                *dataset, _corrDataset,
                _angleMap,_imageCut,_imageTransform);
      _rgeIntenGlobal.extendBy(lens.rgeInten());
    }
  }

  return _rgeIntenGlobal;
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

  switch (_imageTransform.val) {
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
  i += _imageCut.left; j += _imageCut.top;
}

void Lens::calcSensCorr() {
  _hasNaNs = false;
  if (!_corrDataset) return;

  ASSERT(_dataset.imageSize() == _corrDataset->imageSize())

  QSize size = _corrDataset->imageSize();
  size -= _imageCut.marginSize();
  ASSERT(!size.isEmpty())

  qreal sum = 0;

  uint w = size.width(), h = size.height(),
      di = _imageCut.left, dj = _imageCut.top;

  for_ij(w,h)
    sum += _corrDataset->inten(i+di, j+dj);

  qreal avg = sum / (w * h);

  _intensCorr.fill(size);

  for_ij(w,h) {
    auto inten = _corrDataset->inten(i+di,j+dj);
    qreal fact;

    if (inten > 0) {
      fact = avg / inten;
    } else {
      fact = qQNaN(); _hasNaNs = true;
    }

    _intensCorr.setAt(i,j, fact);
  }
}

void Lens::setNorm(eNorm norm) {
  auto &datasets = _dataset.datasets();

  switch (norm) {
  case normNONE:
    _normFactor = 1;
    break;
  case normDELTA_MONITOR_COUNT:
    // could be NaN (divide by 0)
    _normFactor = datasets.avgDeltaMonitorCount() / _dataset.deltaMonitorCount();
    break;
  case normDELTA_TIME:
    // could be NaN (divide by 0)
    _normFactor = datasets.avgDeltaTime() / _dataset.deltaTime();
    break;
  case normBACKGROUND:
    // could be NaN (divide by 0)
    _normFactor = _session.calcAvgBackground(datasets) /
                  _session.calcAvgBackground(_dataset);
    break;
  }
}

}

// eof
