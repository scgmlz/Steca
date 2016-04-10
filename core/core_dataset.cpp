// ************************************************************************** //
//
//  STeCa2:    StressTexCalculator ver. 2
//
//! @file      core_dataset.h
//!
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016
//! @authors   Scientific Computing Group at MLZ Garching
//! @authors   Original version: Christian Randau
//! @authors   Version 2: Antti Soininen, Jan Burle, Rebecca Brydon
//
// ************************************************************************** //

#include "core_dataset.h"

//>>>#include "core_file.h"
//#include "core_session.h"
//#include "core_reflection_info.h"
//#include "core_fit_fitting.h"
//#include "types/core_type_curve.h"
//#include <qmath.h>

namespace core {
//------------------------------------------------------------------------------
// dataset attributes

enum {
  attrDATE, attrCOMMENT,

  attrMOTOR_XT,  attrMOTOR_YT,  attrMOTOR_ZT,
  attrMOTOR_OMG, attrMOTOR_TTH, attrMOTOR_PHI, attrMOTOR_CHI,
  attrMOTOR_PST, attrMOTOR_SST, attrMOTOR_OMGM,
  attrDELTA_MONITOR_COUNT, attrDELTA_TIME,

  NUM_ATTRIBUTES
};

uint Dataset::numAttributes() {
  return NUM_ATTRIBUTES;
}

rcstr Dataset::attributeTag(uint i) {
  static QVector<str> const attributeTags = {
    "date", "comment",
    "X", "Y", "Z",
    "ω", "2θ", "φ", "χ",
    "PST", "SST", "ΩM",
    "Δmon", "Δt",
  };

  return attributeTags.at(i);
}

Dataset::Dataset(
  rcstr date, rcstr comment,
  qreal motorXT,  qreal motorYT,  qreal motorZT,
  qreal motorOmg, qreal motorTth, qreal motorPhi, qreal motorChi,
  qreal motorPST, qreal motorSST, qreal motorOMGM,
  qreal deltaMonitorCount, qreal deltaTime,
  QSize const& size, inten_t const* intens)

: _datasets(nullptr), _date(date), _comment(comment)
, _motorXT(motorXT), _motorYT(motorYT), _motorZT(motorZT)
, _motorOmg(motorOmg), _motorTth(motorTth), _motorPhi(motorPhi), _motorChi(motorChi)
, _motorPST(motorPST), _motorSST(motorSST), _motorOMGM(motorOMGM)
, _deltaMonitorCount(deltaMonitorCount), _deltaTime(deltaTime)
, _image(size,intens) {
}

Datasets const& Dataset::datasets() const {
  ASSERT(_datasets)
  return *_datasets;
}

str Dataset::attributeStrValue(uint i) const {
  qreal value = 0;

  switch (i) {
  case attrDATE:        return _date;
  case attrCOMMENT:     return _comment;

  case attrMOTOR_XT:    value = _motorXT;   break;
  case attrMOTOR_YT:    value = _motorYT;   break;
  case attrMOTOR_ZT:    value = _motorZT;   break;
  case attrMOTOR_OMG:   value = _motorOmg;  break;
  case attrMOTOR_TTH:   value = _motorTth;  break;
  case attrMOTOR_PHI:   value = _motorPhi;  break;
  case attrMOTOR_CHI:   value = _motorChi;  break;
  case attrMOTOR_PST:   value = _motorPST;  break;
  case attrMOTOR_SST:   value = _motorSST;  break;
  case attrMOTOR_OMGM:  value = _motorOMGM; break;
  case attrDELTA_MONITOR_COUNT: value = _deltaMonitorCount; break;
  case attrDELTA_TIME:  value = _deltaTime; break;
  }

  return str::number(value);
}

qreal Dataset::midTth() const {
  return _motorTth;
}

qreal Dataset::deltaMonitorCount() const {
  return _deltaMonitorCount;
}

qreal Dataset::deltaTime() const {
  return _deltaTime;
}

QSize Dataset::imageSize() const {
  return _image.size();
}

inten_t Dataset::inten(uint i, uint j) const {
  return _image.inten(i,j);
}

void Dataset::addIntens(rcDataset that) {
  _image.addIntens(that._image);
}

//------------------------------------------------------------------------------

Datasets::Datasets() {
  invalidateMutables();
}

void Datasets::append(shp_Dataset dataset) {
  ASSERT(!dataset->_datasets) // not appended yet
  dataset->_datasets = this;
  super::append(dataset);

  invalidateMutables();
}

void Datasets::fold() {
  while (super::count() > 1) {
    super::first()->addIntens(*super::last());
    super::removeLast();
  }

  invalidateMutables();
}

QSize Datasets::imageSize() const {
  if (super::isEmpty()) return QSize(0,0);
  // guaranteed that all images have the same size; simply take the first one
  return super::first()->imageSize();
}

//rcRange Datasets::rgeInten() const {
//  if (!_rgeInten.isValid()) {
//    for (auto const& dataset: *this)
//      _rgeInten.extendBy(dataset->image().rgeInten());
//  }

//  return _rgeInten;
//}

qreal Datasets::avgDeltaMonitorCount() const {
  if (qIsNaN(_avgMonitorCount)) {
    qreal avg = 0;
    if (!super::isEmpty()) {
      for (auto const& dataset: *this)
        avg += dataset->deltaMonitorCount();
      avg /= super::count();
    }
    _avgMonitorCount = avg;
  }

  return _avgMonitorCount;
}

qreal Datasets::avgDeltaTime() const {
  if (qIsNaN(_avgDeltaTime)) {
    qreal avg = 0;
    if (!super::isEmpty()) {
      for (auto const& dataset: *this)
        avg += dataset->deltaTime();
      avg /= super::count();
    }
    _avgDeltaTime = avg;
  }

  return _avgDeltaTime;
}

void Datasets::invalidateMutables() {
  _rgeInten.invalidate();
  _avgMonitorCount = _avgDeltaTime = qQNaN();
}

//------------------------------------------------------------------------------
}
// eof
