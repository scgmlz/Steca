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
#include <QVariant>

namespace core {
//------------------------------------------------------------------------------
// metadata attributes

enum class attr {
  DATE, COMMENT,

  MOTOR_XT,  MOTOR_YT,  MOTOR_ZT,
  MOTOR_OMG, MOTOR_TTH, MOTOR_PHI, MOTOR_CHI,
  MOTOR_PST, MOTOR_SST, MOTOR_OMGM,
  DELTA_MONITOR_COUNT, DELTA_TIME,

  NUM_ATTRIBUTES
};

uint Metadata::numAttributes() {
  return (uint)attr::NUM_ATTRIBUTES;
}

rcstr Metadata::attributeTag(uint i) {
  return attributeTags().at(i);
}

str_lst Metadata::attributeTags() {
  static str_lst const tags = {
    "date", "comment",
    "X", "Y", "Z",
    "ω", "2θ", "φ", "χ",
    "PST", "SST", "ΩM",
    "Δmon", "Δt",
  };

  return tags;
}

cmp_vec Metadata::attributeCmps() {
  static cmp_vec const cmps = {
    cmp_date, cmp_str,
    cmp_real, cmp_real, cmp_real,
    cmp_real, cmp_real, cmp_real, cmp_real,
    cmp_real, cmp_real, cmp_real,
    cmp_real, cmp_real,
  };

  return cmps;
}

str Metadata::attributeStrValue(uint i) const {
  qreal value = 0;

  switch ((attr)i) {
  case attr::DATE:        return date;
  case attr::COMMENT:     return comment;

  case attr::MOTOR_XT:    value = motorXT;   break;
  case attr::MOTOR_YT:    value = motorYT;   break;
  case attr::MOTOR_ZT:    value = motorZT;   break;
  case attr::MOTOR_OMG:   value = motorOmg;  break;
  case attr::MOTOR_TTH:   value = motorTth;  break;
  case attr::MOTOR_PHI:   value = motorPhi;  break;
  case attr::MOTOR_CHI:   value = motorChi;  break;
  case attr::MOTOR_PST:   value = motorPST;  break;
  case attr::MOTOR_SST:   value = motorSST;  break;
  case attr::MOTOR_OMGM:  value = motorOMGM; break;
  case attr::DELTA_MONITOR_COUNT: value = deltaMonitorCount; break;
  case attr::DELTA_TIME:  value = deltaTime; break;
  default: NEVER_HERE
  }

  return str::number(value);
}

QVariant Metadata::attributeValue(uint i) const {
  switch ((attr)i) {
  case attr::DATE:       return date;
  case attr::COMMENT:    return comment;
  case attr::MOTOR_XT:   return (qreal)motorXT;
  case attr::MOTOR_YT:   return (qreal)motorYT;
  case attr::MOTOR_ZT:   return (qreal)motorZT;
  case attr::MOTOR_OMG:  return (qreal)motorOmg;
  case attr::MOTOR_TTH:  return (qreal)motorTth;
  case attr::MOTOR_PHI:  return (qreal)motorPhi;
  case attr::MOTOR_CHI:  return (qreal)motorChi;
  case attr::MOTOR_PST:  return (qreal)motorPST;
  case attr::MOTOR_SST:  return (qreal)motorSST;
  case attr::MOTOR_OMGM: return (qreal)motorOMGM;
  case attr::DELTA_MONITOR_COUNT: return deltaMonitorCount;
  case attr::DELTA_TIME: return deltaTime;
  default:
    NEVER_HERE
    return 0;
  }
}

row_t Metadata::attributeValues() const {
  row_t attrs;
  for_i ((uint)attr::NUM_ATTRIBUTES)
    attrs.append(attributeValue(i));
  return attrs;
}

row_t Metadata::attributeNaNs() {
  static row_t row;
  if (row.isEmpty()) {
    for_i ((uint)attr::NUM_ATTRIBUTES)
      row.append(qQNaN());
  }
  return row;
}

//------------------------------------------------------------------------------

Dataset::Dataset(rcMetadata md, QSize const& size, inten_t const* intens)
: datasets_(nullptr), md_(new Metadata(md)), image_(size,intens) {
}

Dataset::Dataset(rcDataset that)
: datasets_(nullptr), md_(that.md_), image_(that.image_) {
}

rcDatasets Dataset::datasets() const {
  ASSERT(datasets_)
  return *datasets_;
}

shp_Metadata Dataset::metadata() const {
  return md_;
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
    rcMetadata md = *(d->md_);
    motorXT   += md.motorXT;
    motorYT   += md.motorYT;
    motorZT   += md.motorZT;

    motorOmg  += md.motorOmg;
    motorTth  += md.motorTth;
    motorPhi  += md.motorPhi;
    motorChi  += md.motorChi;

    motorPST  += md.motorPST;
    motorSST  += md.motorSST;
    motorOMGM += md.motorOMGM;

    deltaMonitorCount += md.deltaMonitorCount;
    deltaTime         += md.deltaTime;
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

  Metadata md;

  md.date    = first.md_->date;
  md.comment = first.md_->comment;

  md.motorXT  = motorXT;
  md.motorYT  = motorYT;
  md.motorZT  = motorZT;
  md.motorOmg = motorOmg;
  md.motorTth = motorTth;
  md.motorPhi = motorPhi;
  md.motorChi = motorChi;
  md.motorPST = motorPST;
  md.motorSST = motorSST;
  md.motorOMGM  = motorOMGM;
  md.deltaMonitorCount = deltaMonitorCount;
  md.deltaTime  = deltaTime;

  return shp_Dataset(
    new Dataset(md, image.size(), image.intensData()));
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
