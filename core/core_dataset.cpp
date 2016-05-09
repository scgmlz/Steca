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
#include "core_session.h"
#include "core_lens.h"
#include <QStringList>

namespace core {
//------------------------------------------------------------------------------
// dataset attributes

enum class attr {
  DATE, COMMENT,

  MOTOR_XT,  MOTOR_YT,  MOTOR_ZT,
  MOTOR_OMG, MOTOR_TTH, MOTOR_PHI, MOTOR_CHI,
  MOTOR_PST, MOTOR_SST, MOTOR_OMGM,
  DELTA_MONITOR_COUNT, DELTA_TIME,

  NUM_ATTRIBUTES
};

uint Dataset::numAttributes() {
  return (uint)attr::NUM_ATTRIBUTES;
}

rcstr Dataset::attributeTag(uint i) {
  return attributeTags().at(i);
}

str_lst Dataset::attributeTags() {
  static str_lst const tags = {
    "date", "comment",
    "X", "Y", "Z",
    "ω", "2θ", "φ", "χ",
    "PST", "SST", "ΩM",
    "Δmon", "Δt",
  };

  return tags;
}

cmp_vec Dataset::attributeCmps() {
  static cmp_vec const cmps = {
    cmp_date, cmp_str,
    cmp_real, cmp_real, cmp_real,
    cmp_real, cmp_real, cmp_real, cmp_real,
    cmp_real, cmp_real, cmp_real,
    cmp_real, cmp_real,
  };

  return cmps;
}

//------------------------------------------------------------------------------

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

Dataset::Dataset(rcDataset that)
: datasets_(nullptr), date_(that.date_), comment_(that.comment_)
, motorXT_(that.motorXT_), motorYT_(that.motorYT_), motorZT_(that.motorZT_)
, motorOmg_(that.motorOmg_), motorTth_(that.motorTth_), motorPhi_(that.motorPhi_), motorChi_(that.motorChi_)
, motorPST_(that.motorPST_), motorSST_(that.motorSST_), motorOMGM_(that.motorOMGM_)
, deltaMonitorCount_(that.deltaMonitorCount_), deltaTime_(that.deltaTime_)
, image_(that.image_) {
}

rcDatasets Dataset::datasets() const {
  ASSERT(datasets_)
  return *datasets_;
}

shp_Dataset Dataset::combine(Datasets datasets) {
  if (datasets.count() <= 0)
    return shp_Dataset();

  qreal motorXT = 0, motorYT = 0,  motorZT = 0,
        motorOmg = 0, motorTth = 0, motorPhi = 0, motorChi = 0,
        motorPST = 0, motorSST = 0, motorOMGM = 0,
        deltaMonitorCount = 0, deltaTime = 0;

  // sums: deltaMonitorCount, deltaTime
  // the rest are averaged

  for (auto const& d: datasets) {
    motorXT   += d->motorXT_;
    motorYT   += d->motorYT_;
    motorZT   += d->motorZT_;

    motorOmg  += d->motorOmg_;
    motorTth  += d->motorTth_;
    motorPhi  += d->motorPhi_;
    motorChi  += d->motorChi_;

    motorPST  += d->motorPST_;
    motorSST  += d->motorSST_;
    motorOMGM += d->motorOMGM_;

    deltaMonitorCount += d->deltaMonitorCount_;
    deltaTime         += d->deltaTime_;
  }

  uint count = datasets.count();
  motorXT  /= count;
  motorYT  /= count;
  motorZT  /= count;

  motorOmg /= count;
  motorTth /= count;
  motorPhi /= count;
  motorChi /= count;

  motorPST  /= count;
  motorSST  /= count;
  motorOMGM /= count;

  // added intensities
  Image image = datasets.folded();

  // take string attributes from the first dataset
  auto const& first = *datasets.at(0);

  rcstr date    = first.date_;
  rcstr comment = first.comment_;

  return shp_Dataset(
    new Dataset(date, comment,
                motorXT, motorYT,  motorZT,  motorOmg, motorTth,
                motorPhi, motorChi, motorPST, motorSST, motorOMGM,
                deltaMonitorCount, deltaTime,
                image.size(), image.intensData()));
}

str Dataset::attributeStrValue(uint i) const {
  qreal value = 0;

  switch ((attr)i) {
  case attr::DATE:        return date_;
  case attr::COMMENT:     return comment_;

  case attr::MOTOR_XT:    value = motorXT_;   break;
  case attr::MOTOR_YT:    value = motorYT_;   break;
  case attr::MOTOR_ZT:    value = motorZT_;   break;
  case attr::MOTOR_OMG:   value = motorOmg_;  break;
  case attr::MOTOR_TTH:   value = motorTth_;  break;
  case attr::MOTOR_PHI:   value = motorPhi_;  break;
  case attr::MOTOR_CHI:   value = motorChi_;  break;
  case attr::MOTOR_PST:   value = motorPST_;  break;
  case attr::MOTOR_SST:   value = motorSST_;  break;
  case attr::MOTOR_OMGM:  value = motorOMGM_; break;
  case attr::DELTA_MONITOR_COUNT: value = deltaMonitorCount_; break;
  case attr::DELTA_TIME:  value = deltaTime_; break;
  default: NEVER_HERE
  }

  return str::number(value);
}

QVariant Dataset::attributeValue(uint i) const {
  switch ((attr)i) {
  case attr::DATE:       return date_;
  case attr::COMMENT:    return comment_;
  case attr::MOTOR_XT:   return (qreal)motorXT_;
  case attr::MOTOR_YT:   return (qreal)motorYT_;
  case attr::MOTOR_ZT:   return (qreal)motorZT_;
  case attr::MOTOR_OMG:  return (qreal)motorOmg_;
  case attr::MOTOR_TTH:  return (qreal)motorTth_;
  case attr::MOTOR_PHI:  return (qreal)motorPhi_;
  case attr::MOTOR_CHI:  return (qreal)motorChi_;
  case attr::MOTOR_PST:  return (qreal)motorPST_;
  case attr::MOTOR_SST:  return (qreal)motorSST_;
  case attr::MOTOR_OMGM: return (qreal)motorOMGM_;
  case attr::DELTA_MONITOR_COUNT: return deltaMonitorCount_;
  case attr::DELTA_TIME: return deltaTime_;
  default:
    NEVER_HERE
    return 0;
  }
}

row_t Dataset::attributeValues() const {
  row_t attrs;
  for_i ((uint)attr::NUM_ATTRIBUTES)
    attrs.append(attributeValue(i));
  return attrs;
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

void Datasets::appendHere(shp_Dataset dataset) {
  // a dataset can be added to Datasets only once
  ASSERT(!dataset->datasets_)
  dataset->datasets_ = this;

  super::append(dataset);
  invalidateMutables();
}

Image Datasets::folded() const THROWS {
  ASSERT(0 < count())

  Image image(first()->imageSize());
  for (auto const& dataset: *this)
    image.addIntens(dataset->image_);

  return image;
}

QSize Datasets::imageSize() const {
  if (super::isEmpty())
    return QSize(0,0);

  // guaranteed that all images have the same size; simply take the first one
  return super::first()->imageSize();
}

qreal Datasets::avgDeltaMonitorCount() const {
  if (qIsNaN(avgMonitorCount_)) {
    qreal avg = 0;

    for (auto const& dataset: *this)
      avg += dataset->deltaMonitorCount();

    if (!super::isEmpty())
      avg /= super::count();

    avgMonitorCount_ = avg;
  }

  return avgMonitorCount_;
}

qreal Datasets::avgDeltaTime() const {
  if (qIsNaN(avgDeltaTime_)) {
    qreal avg = 0;

    for (auto const& dataset: *this)
      avg += dataset->deltaTime();

    if (!super::isEmpty())
      avg /= super::count();

    avgDeltaTime_ = avg;
  }

  return avgDeltaTime_;
}

rcRange Datasets::rgeFixedInten(rcSession session, bool trans, bool cut) const {
  if (!rgeFixedInten_.isValid()) {

    TakesLongTime __;

    for (auto const& dataset: *this) {
      auto const& image = dataset->image();
      shp_ImageLens imageLens = session.lens(image,*this,trans,cut);
      rgeFixedInten_.extendBy(imageLens->rgeInten(false));
    }
  }

  return rgeFixedInten_;
}

rcCurve Datasets::makeAvgCurve(rcSession session, bool trans, bool cut) const {
  if (!avgCurve_.isEmpty())
    return avgCurve_;

  TakesLongTime __;

  Curve res;

  for (auto const& dataset: *this) {
    shp_Lens lens = session.lens(*dataset,*this,trans,cut,session.norm());
    Curve single = lens->makeCurve(lens->angleMap().rgeGamma(),lens->angleMap().rgeTth());
    res = res.add(single);
  }

  if (!isEmpty())
    res = res.mul(1. / count());

  return (avgCurve_ = res);
}

void Datasets::invalidateMutables() {
  avgMonitorCount_ = avgDeltaTime_ = qQNaN();
  rgeFixedInten_.invalidate();
  avgCurve_.clear();
}

//------------------------------------------------------------------------------
}
// eof
