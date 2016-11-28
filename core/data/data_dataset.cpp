// ************************************************************************** //
//
// STeCa2 - StressTextureCalculator ver. 2
//
// Copyright (C) 2016 Forschungszentrum Jülich GmbH 2016
//
// This program is free software: you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the Free
// Software Foundation, either version 3 of the License, or (at your option)
// any later version.
//
// This program is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
// or FITNESS FOR A PARTICULAR PURPOSE.
//
// See the COPYING and AUTHORS files for more details.
// ************************************************************************** //

#include "data_dataset.h"
#include "calc/calc_lens.h"
#include "session.h"
#include "typ/typ_log.h"
#include <qmath.h>

namespace data {
//------------------------------------------------------------------------------
// metadata attributes

using namespace::typ;
using namespace::calc;

enum class eAttr {
  MOTOR_XT,  MOTOR_YT,  MOTOR_ZT,
  MOTOR_OMG, MOTOR_TTH, MOTOR_PHI, MOTOR_CHI,
  MOTOR_PST, MOTOR_SST, MOTOR_OMGM,
  MONITOR_COUNT, DELTA_MONITOR_COUNT,
  TIME, DELTA_TIME,

  NUM_NUMERICAL_ATTRIBUTES,

  // non-numbers must come last
  DATE = NUM_NUMERICAL_ATTRIBUTES, COMMENT,

  NUM_ALL_ATTRIBUTES
};

Metadata::Metadata()
: date(), comment()
, motorXT(0), motorYT(0), motorZT(0)
, motorOmg(0), motorTth(0), motorPhi(0), motorChi(0)
, motorPST(0), motorSST(0), motorOMGM(0)
, monitorCount(0), deltaMonitorCount(0)
, time(0), deltaTime(0) {
}

uint Metadata::numAttributes(bool onlyNum) {
  return uint(onlyNum
      ? eAttr::NUM_NUMERICAL_ATTRIBUTES
      : eAttr::NUM_ALL_ATTRIBUTES);
}

rcstr Metadata::attributeTag(uint i, bool out) {
  return attributeTags(out).at(i);
}

str_lst Metadata::attributeTags(bool out) {
  static str_lst const tags = {
    "X", "Y", "Z",
    "ω", "mid 2θ", "φ", "χ",
    "PST", "SST", "ΩM",
    "mon", "Δmon",
    "t", "Δt",
    "date", "comment",
  };

  static str_lst const outTags = {
    "X", "Y", "Z",
    "omega", "mid2theta", "phi", "chi",
    "PST", "SST", "OmegaM",
    "mon", "delta_mon",
    "t", "delta_t",
    "date", "comment",
  };

  return out ? outTags : tags;
}

cmp_vec Metadata::attributeCmps() {
  static cmp_vec const cmps = {
    cmp_real, cmp_real, cmp_real,
    cmp_real, cmp_real, cmp_real, cmp_real,
    cmp_real, cmp_real, cmp_real,
    cmp_real, cmp_real,
    cmp_real, cmp_real,
    cmp_date, cmp_str,
  };

  return cmps;
}

str Metadata::attributeStrValue(uint i) const {
  qreal value = 0;

  switch (eAttr(i)) {
  case eAttr::MOTOR_XT:    value = motorXT;   break;
  case eAttr::MOTOR_YT:    value = motorYT;   break;
  case eAttr::MOTOR_ZT:    value = motorZT;   break;
  case eAttr::MOTOR_OMG:   value = motorOmg;  break;
  case eAttr::MOTOR_TTH:   value = motorTth;  break;
  case eAttr::MOTOR_PHI:   value = motorPhi;  break;
  case eAttr::MOTOR_CHI:   value = motorChi;  break;
  case eAttr::MOTOR_PST:   value = motorPST;  break;
  case eAttr::MOTOR_SST:   value = motorSST;  break;
  case eAttr::MOTOR_OMGM:  value = motorOMGM; break;
  case eAttr::MONITOR_COUNT: value = monitorCount; break;
  case eAttr::DELTA_MONITOR_COUNT: value = deltaMonitorCount; break;
  case eAttr::TIME:        value = time; break;
  case eAttr::DELTA_TIME:  value = deltaTime; break;

  case eAttr::DATE:        return date;
  case eAttr::COMMENT:     return comment;

  default: NEVER;
  }

  return str::number(value);
}

QVariant Metadata::attributeValue(uint i) const {
  switch (eAttr(i)) {
  case eAttr::DATE:       return date;
  case eAttr::COMMENT:    return comment;
  case eAttr::MOTOR_XT:   return qreal(motorXT);
  case eAttr::MOTOR_YT:   return qreal(motorYT);
  case eAttr::MOTOR_ZT:   return qreal(motorZT);
  case eAttr::MOTOR_OMG:  return qreal(motorOmg);
  case eAttr::MOTOR_TTH:  return qreal(motorTth);
  case eAttr::MOTOR_PHI:  return qreal(motorPhi);
  case eAttr::MOTOR_CHI:  return qreal(motorChi);
  case eAttr::MOTOR_PST:  return qreal(motorPST);
  case eAttr::MOTOR_SST:  return qreal(motorSST);
  case eAttr::MOTOR_OMGM: return qreal(motorOMGM);
  case eAttr::MONITOR_COUNT: return monitorCount;
  case eAttr::DELTA_MONITOR_COUNT: return deltaMonitorCount;
  case eAttr::TIME:       return time;
  case eAttr::DELTA_TIME: return deltaTime;
  default:
    NEVER return 0;
  }
}

row_t Metadata::attributeValues() const {
  row_t attrs;
  for_i (uint(eAttr::NUM_ALL_ATTRIBUTES))
    attrs.append(attributeValue(i));
  return attrs;
}

row_t Metadata::attributeNaNs() {
  static row_t row;
  if (row.isEmpty())
    for_i (uint(eAttr::NUM_ALL_ATTRIBUTES))
      row.append(qQNaN());
  return row;
}

//------------------------------------------------------------------------------

OneDataset::OneDataset(Metadata::rc md, size2d::rc size, not_null<inten_t const*> intens)
: md_(new Metadata(md)), image_(size,intens) {
}

OneDataset::OneDataset(rc that)
: md_(that.md_), image_(that.image_) {
}

shp_Metadata OneDataset::metadata() const {
  ENSURE(!md_.isNull())
  return md_;
}

gma_rge OneDataset::rgeGma(core::Session::rc session) const {
  return session.angleMap(*this)->rgeGma();
}

gma_rge OneDataset::rgeGmaFull(core::Session::rc session) const {
  return session.angleMap(*this)->rgeGmaFull();
}

tth_rge OneDataset::rgeTth(core::Session::rc session) const {
  return session.angleMap(*this)->rgeTth();
}

inten_rge OneDataset::rgeInten() const {
  return image_.rgeInten();
}

size2d OneDataset::imageSize() const {
  return image_.size();
}

void OneDataset::collectIntens(core::Session::rc session, typ::Image const* intensCorr,
                               inten_vec& intens, uint_vec& counts,gma_rge::rc rgeGma,
                               tth_t minTth, tth_t deltaTth) const {
  auto angleMap = session.angleMap(*this);
  EXPECT(!angleMap.isNull())
  AngleMap::rc map = *angleMap;

  uint_vec const* gmaIndexes = nullptr;
  uint gmaIndexMin = 0, gmaIndexMax = 0;
  map.getGmaIndexes(rgeGma, gmaIndexes, gmaIndexMin, gmaIndexMax);

  EXPECT(gmaIndexes)
  EXPECT(gmaIndexMin <= gmaIndexMax)
  EXPECT(gmaIndexMax <= gmaIndexes->count())

  EXPECT(intens.count() == counts.count())
  uint count = intens.count();

  EXPECT(0 < deltaTth)

  for (uint i = gmaIndexMin; i < gmaIndexMax; ++i) {
    uint ind = gmaIndexes->at(i);
    inten_t inten = image_.inten(ind);
    if (qIsNaN(inten))
      continue;

    inten_t corr = intensCorr ? intensCorr->at(ind) : 1;
    if (qIsNaN(corr))
      continue;

    inten *= corr;

    tth_t tth  = map.at(ind).tth;

    // bin index
    uint ti = to_u(qFloor((tth - minTth) / deltaTth));
    EXPECT(ti <= count)
    ti = qMin(ti, count-1); // it can overshoot due to floating point calculation

    intens[ti] += inten;
    counts[ti] += 1;
  }
}

//------------------------------------------------------------------------------

Dataset::Dataset()
: datasets_(nullptr) {
}

shp_Metadata Dataset::metadata() const {
  if (md_.isNull()) {
    EXPECT(!isEmpty())
    const_cast<Cls*>(this)->md_ = shp_Metadata(new Metadata);
    Metadata *m = const_cast<Metadata*>(md_.data());

    EXPECT(!first()->metadata().isNull())
    Metadata::rc firstMd = *(first()->metadata());

    m->date         = firstMd.date;
    m->comment      = firstMd.comment;

    // sums: delta mon. count and time,
    // takes the last ones (presumed the maximum) of mon. count and time,
    // averages the rest
    for (auto& one : *this) {
       Metadata const* d = one->metadata().data();
       EXPECT(d)

       m->motorXT   += d->motorXT;
       m->motorYT   += d->motorYT;
       m->motorZT   += d->motorZT;

       m->motorOmg  += d->motorOmg;
       m->motorTth  += d->motorTth;
       m->motorPhi  += d->motorPhi;
       m->motorChi  += d->motorChi;

       m->motorPST  += d->motorPST;
       m->motorSST  += d->motorSST;
       m->motorOMGM += d->motorOMGM;

       m->deltaMonitorCount += d->deltaMonitorCount;
       m->deltaTime    += d->deltaTime;

       if (m->monitorCount > d->monitorCount)
         MessageLogger::warn("decreasing monitor count in combined datasets");
       if (m->time > d->time)
         MessageLogger::warn("decreasing time in combined datasets");
       m->monitorCount = d->monitorCount;
       m->time         = d->time;
    }

    qreal fac = 1.0 / count();

    m->motorXT   *= fac;
    m->motorYT   *= fac;
    m->motorZT   *= fac;

    m->motorOmg  *= fac;
    m->motorTth  *= fac;
    m->motorPhi  *= fac;
    m->motorChi  *= fac;

    m->motorPST  *= fac;
    m->motorSST  *= fac;
    m->motorOMGM *= fac;
  }

  return md_;
}

Datasets::rc Dataset::datasets() const {
  EXPECT(datasets_)
  return *datasets_;
}

#define AVG_ONES(what)    \
  EXPECT(!isEmpty())      \
  qreal avg = 0;          \
  for (auto& one : *this) \
    avg += one->what();   \
  avg /= count();         \
  return avg;

deg Dataset::omg() const {
  AVG_ONES(omg)
}

deg Dataset::phi() const {
  AVG_ONES(phi)
}

deg Dataset::chi() const {
  AVG_ONES(chi)
}

// combined range of combined datasets
#define RGE_COMBINE(combineOp, what)  \
  EXPECT(!isEmpty())                  \
  Range rge;                          \
  for (auto& one : *this)             \
    rge.combineOp(one->what);         \
  return rge;

gma_rge Dataset::rgeGma(core::Session::rc session) const {
  RGE_COMBINE(extendBy, rgeGma(session))
}

gma_rge Dataset::rgeGmaFull(core::Session::rc session) const {
  RGE_COMBINE(extendBy, rgeGmaFull(session))
}

tth_rge Dataset::rgeTth(core::Session::rc session) const {
  RGE_COMBINE(extendBy, rgeTth(session))
}

inten_rge Dataset::rgeInten() const {
  RGE_COMBINE(intersect, rgeInten())
}

qreal Dataset::avgMonitorCount() const {
  AVG_ONES(monitorCount)
}

qreal Dataset::avgDeltaMonitorCount() const {
  AVG_ONES(deltaMonitorCount)
}

qreal Dataset::avgDeltaTime() const {
  AVG_ONES(deltaTime)
}

inten_vec Dataset::collectIntens(
    core::Session::rc session, typ::Image const* intensCorr, gma_rge::rc rgeGma,
    bool averaged) const
{
  tth_rge tthRge = rgeTth(session);
  tth_t   tthWdt = tthRge.width();

  auto cut = session.imageCut();
  uint pixWidth = session.imageSize().w - cut.left - cut.right;

  uint numBins;
  if (1 < count()) {  // combined datasets
    auto one = first();
    tth_t delta = one->rgeTth(session).width() / pixWidth;
    numBins = to_u(qCeil(tthWdt / delta));
  } else {
    numBins = pixWidth; // simply match the pixels
  }

  inten_vec intens(numBins, 0);
  uint_vec  counts(numBins, 0);

  tth_t minTth = tthRge.min, deltaTth = tthWdt / numBins;

  for (auto& one : *this)
    one->collectIntens(session, intensCorr, intens, counts, rgeGma, minTth, deltaTth);

  if (averaged)
    for_i (numBins) {
      auto cnt = counts.at(i);
      if (cnt > 0)
        intens[i] /= cnt;
    }

  return intens;
}

size2d Dataset::imageSize() const {
  EXPECT(!isEmpty())
  // all images have the same size; simply take the first one
  return first()->imageSize();
}

//------------------------------------------------------------------------------

Datasets::Datasets() {
  invalidateAvgMutables();
}

void Datasets::appendHere(shp_Dataset dataset) {
  // can be added only once
  EXPECT(!dataset->datasets_)
  dataset->datasets_ = this;

  super::append(dataset);
  invalidateAvgMutables();
}

size2d Datasets::imageSize() const {
  if (isEmpty())
    return size2d(0,0);

  // all images have the same size; simply take the first one
  return first()->imageSize();
}

qreal Datasets::avgMonitorCount() const {
  if (qIsNaN(avgMonitorCount_))
    avgMonitorCount_ = calcAvgMutable(&Dataset::avgMonitorCount);

  return avgMonitorCount_;
}

qreal Datasets::avgDeltaMonitorCount() const {
  if (qIsNaN(avgDeltaMonitorCount_))
    avgDeltaMonitorCount_ = calcAvgMutable(&Dataset::avgDeltaMonitorCount);

  return avgDeltaMonitorCount_;
}

qreal Datasets::avgDeltaTime() const {
  if (qIsNaN(avgDeltaTime_))
    avgDeltaTime_ = calcAvgMutable(&Dataset::avgDeltaTime);

  return avgDeltaTime_;
}

inten_rge::rc Datasets::rgeGma(core::Session::rc session) const {
  if (!rgeGma_.isValid())
    for (auto& dataset: *this)
      rgeGma_.extendBy(dataset->rgeGma(session));

  return rgeGma_;
}

inten_rge::rc Datasets::rgeFixedInten(core::Session::rc session, bool trans, bool cut) const {
  if (!rgeFixedInten_.isValid()) {

    TakesLongTime __;

    for (auto& dataset: *this)
      for (auto& one : *dataset) {
        auto& image = one->image();
        shp_ImageLens imageLens = session.imageLens(image,*this,trans,cut);
        rgeFixedInten_.extendBy(imageLens->rgeInten(false));
      }
  }

  return rgeFixedInten_;
}

Curve Datasets::avgCurve(core::Session::rc session, bool averaged) const {
  if (avgCurve_.isEmpty()) {
    // TODO invalidate when combinedDgram is unchecked

    TakesLongTime __;

    avgCurve_ = session.datasetLens(*combineAll(), *this, session.norm(), true, true)->makeCurve(averaged);
  }

  return avgCurve_;
}

void Datasets::invalidateAvgMutables() const {
  avgMonitorCount_ = avgDeltaMonitorCount_ = avgDeltaTime_ = qQNaN();
  rgeFixedInten_.invalidate();
  rgeGma_.invalidate();
  avgCurve_.clear();
}

shp_Dataset Datasets::combineAll() const {
  shp_Dataset d(new Dataset);

  for (shp_Dataset const& dataset: *this)
    for (shp_OneDataset const& one: *dataset)
      d->append(one);

  return d;
}

qreal Datasets::calcAvgMutable(qreal (Dataset::*avgMth)() const) const {
  qreal avg = 0;

  if (!isEmpty()) {
    for (auto& dataset: *this)
      avg += ((*dataset).*avgMth)();

    avg /= super::count();
  }

  return avg;
}

size2d OneDatasets::imageSize() const {
  EXPECT(!isEmpty())
  // all images have the same size; simply take the first one
  return first()->imageSize();
}

Image OneDatasets::foldedImage() const {
  EXPECT(!isEmpty())
  Image image(imageSize());

  for (auto& one: *this)
    image.addIntens(one->image_);

  return image;
}

//------------------------------------------------------------------------------
}
// eof
