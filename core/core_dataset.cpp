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

: datasets_(nullptr), date_(date), comment_(comment)
, motorXT_(motorXT), motorYT_(motorYT), motorZT_(motorZT)
, motorOmg_(motorOmg), motorTth_(motorTth), motorPhi_(motorPhi), motorChi_(motorChi)
, motorPST_(motorPST), motorSST_(motorSST), motorOMGM_(motorOMGM)
, deltaMonitorCount_(deltaMonitorCount), deltaTime_(deltaTime)
, image_(size,intens) {
}

Datasets const& Dataset::datasets() const {
  ASSERT(datasets_)
  return *datasets_;
}

str Dataset::attributeStrValue(uint i) const {
  qreal value = 0;

  switch (i) {
  case attrDATE:        return date_;
  case attrCOMMENT:     return comment_;

  case attrMOTOR_XT:    value = motorXT_;   break;
  case attrMOTOR_YT:    value = motorYT_;   break;
  case attrMOTOR_ZT:    value = motorZT_;   break;
  case attrMOTOR_OMG:   value = motorOmg_;  break;
  case attrMOTOR_TTH:   value = motorTth_;  break;
  case attrMOTOR_PHI:   value = motorPhi_;  break;
  case attrMOTOR_CHI:   value = motorChi_;  break;
  case attrMOTOR_PST:   value = motorPST_;  break;
  case attrMOTOR_SST:   value = motorSST_;  break;
  case attrMOTOR_OMGM:  value = motorOMGM_; break;
  case attrDELTA_MONITOR_COUNT: value = deltaMonitorCount_; break;
  case attrDELTA_TIME:  value = deltaTime_; break;
  }

  return str::number(value);
}

qreal Dataset::midTth() const {
  return motorTth_;
}

qreal Dataset::deltaMonitorCount() const {
  return deltaMonitorCount_;
}

qreal Dataset::deltaTime() const {
  return deltaTime_;
}

QSize Dataset::imageSize() const {
  return image_.size();
}

inten_t Dataset::inten(uint i, uint j) const {
  return image_.inten(i,j);
}

//------------------------------------------------------------------------------

Datasets::Datasets() {
  invalidateMutables();
}

void Datasets::append(shp_Dataset dataset) {
  ASSERT(!dataset->datasets_) // not appended yet
  dataset->datasets_ = this;
  super::append(dataset);

  invalidateMutables();
}

Image Datasets::folded() THROWS {
  ASSERT(0 < count())
  Image image(first()->imageSize());

  for (auto &dataset: *this)
    image.addIntens(dataset->image_);

  return image;
}

QSize Datasets::imageSize() const {
  if (super::isEmpty()) return QSize(0,0);
  // guaranteed that all images have the same size; simply take the first one
  return super::first()->imageSize();
}

qreal Datasets::avgDeltaMonitorCount() const {
  if (qIsNaN(avgMonitorCount_)) {
    qreal avg = 0;
    if (!super::isEmpty()) {
      for (auto const& dataset: *this)
        avg += dataset->deltaMonitorCount();
      avg /= super::count();
    }
    avgMonitorCount_ = avg;
  }

  return avgMonitorCount_;
}

qreal Datasets::avgDeltaTime() const {
  if (qIsNaN(avgDeltaTime_)) {
    qreal avg = 0;
    if (!super::isEmpty()) {
      for (auto const& dataset: *this)
        avg += dataset->deltaTime();
      avg /= super::count();
    }
    avgDeltaTime_ = avg;
  }

  return avgDeltaTime_;
}

void Datasets::invalidateMutables() {
  avgMonitorCount_ = avgDeltaTime_ = qQNaN();
}

//------------------------------------------------------------------------------
}
// eof
