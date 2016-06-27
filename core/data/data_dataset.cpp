// ************************************************************************** //
//
//  STeCa2:    StressTextureCalculator ver. 2
//
//! @file      core_dataset.h
//!
//! @homepage  http://apps.jcns.fz-juelich.de/steca2
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016
//! @authors   Scientific Computing Group at MLZ Garching
//! @authors   Rebecca Brydon, Jan Burle,  Antti Soininen
//! @authors   Based on the original STeCa by Christian Randau
//
// ************************************************************************** //

#include "data_dataset.h"
#include "calc/calc_lens.h"
#include "session.h"
#include "typ/typ_async.h"
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

rcstr Metadata::attributeTag(uint i) {
  return attributeTags().at(i);
}

str_lst Metadata::attributeTags() {
  static str_lst const tags = {
    "X", "Y", "Z",
    "ω", "mid 2θ", "φ", "χ",
    "PST", "SST", "ΩM",
    "mon", "Δmon",
    "t", "Δt",
    "date", "comment",
  };

  return tags;
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

tth_rge OneDataset::rgeTth(core::Session::rc session) const {
  return session.angleMap(*this)->rgeTth();
}

inten_rge OneDataset::rgeInten() const {
  return image_.rgeInten();
}

size2d OneDataset::imageSize() const {
  return image_.size(); // REVIEW - (how) used ?
}

void OneDataset::collectIntens(core::Session::rc session,
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

  for (uint i = gmaIndexMin; i < gmaIndexMax; ++i) {
    uint ind = gmaIndexes->at(i);
    EXPECT(rgeGma.contains(map.at(ind).gma))
    inten_t inten = image_.inten(ind);
    if (qIsNaN(inten))
      continue;

    uint count = intens.count();
    tth_t tth   = map.at(ind).tth;
    EXPECT(minTth <= tth && tth <= minTth + count*deltaTth)
    uint ti = qMin(to_u(qFloor((tth - minTth) / deltaTth)), count-1); // bin index
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

    m->date    = firstMd.date;
    m->comment = firstMd.comment;

    // sums: mon. count and time, averages the rest
    for (auto& one : *this) {
       Metadata const* o = one->metadata().data();
       EXPECT(o)
       m->motorXT   += o->motorXT;
       m->motorYT   += o->motorYT;
       m->motorZT   += o->motorZT;

       m->motorOmg  += o->motorOmg;
       m->motorTth  += o->motorTth;
       m->motorPhi  += o->motorPhi;
       m->motorChi  += o->motorChi;

       m->motorPST  += o->motorPST;
       m->motorSST  += o->motorSST;
       m->motorOMGM += o->motorOMGM;

       m->deltaMonitorCount += o->deltaMonitorCount;
       m->monitorCount += o->monitorCount;
       m->deltaTime    += o->deltaTime;
       m->time         += o->time;
    }

    qreal fac = 1.0 / count();

    m->motorXT  *= fac;
    m->motorYT  *= fac;
    m->motorZT  *= fac;

    m->motorOmg *= fac;
    m->motorTth *= fac;
    m->motorPhi *= fac;
    m->motorChi *= fac;

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
  for (auto &one : *this) \
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

#define RGE_ONES(what)                \
  EXPECT(!isEmpty())                  \
  Range rge;                          \
  for (auto &one : *this)             \
    rge.extendBy(one->what(session)); \
  return rge;

gma_rge Dataset::rgeGma(core::Session::rc session) const {
  // TODO mutables ?
  RGE_ONES(rgeGma)
}

tth_rge Dataset::rgeTth(core::Session::rc session) const {
  // TODO mutables ?
  RGE_ONES(rgeTth)
}

inten_rge Dataset::rgeInten() const {
  // TODO mutables ?
  EXPECT(!isEmpty())
  Range rge;
  for (auto &one : *this)
    rge.intersect(one->rgeInten());
  return rge;
}

qreal Dataset::avgDeltaMonitorCount() const {
  AVG_ONES(deltaMonitorCount)
}

qreal Dataset::avgDeltaTime() const {
  AVG_ONES(deltaTime)
}

inten_vec Dataset::collectIntens(core::Session::rc session, gma_rge::rc rgeGma) const {
  tth_rge tthRge = rgeTth(session);
  tth_t   tthWdt = tthRge.width();

  auto cut = session.imageCut();
  uint pixWidth = session.imageSize().w - cut.left - cut.right;

  uint numBins;
  if (1 < count()) {  // combined
    auto one = first();
    tth_t delta = one->rgeTth(session).width() / pixWidth;
    numBins = to_u(qCeil(tthWdt / delta / 2));
  } else {
    numBins = pixWidth; // simply match the pixels
  }

  inten_vec intens(numBins, 0);
  uint_vec  counts(numBins, 0);

  tth_t minTth = tthRge.min, deltaTth = tthWdt / numBins;

  for (auto& one : *this)
    one->collectIntens(session, intens, counts, rgeGma, minTth, deltaTth);

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
  invalidateMutables();
}

void Datasets::appendHere(shp_Dataset dataset) {
  // can be added only once
  EXPECT(!dataset->datasets_)
  dataset->datasets_ = this;

  super::append(dataset);
  invalidateMutables();
}

size2d Datasets::imageSize() const {
  if (isEmpty())
    return size2d(0,0);

  // all images have the same size; simply take the first one
  return first()->imageSize();
}

qreal Datasets::avgDeltaMonitorCount() const {
  if (qIsNaN(avgMonitorCount_)) {
    qreal avg = 0;

    if (!isEmpty()) {
      for (auto& dataset: *this)
        avg += dataset->avgDeltaMonitorCount();

      avg /= super::count();
    }

    avgMonitorCount_ = avg;
  }

  return avgMonitorCount_;
}

qreal Datasets::avgDeltaTime() const {
  if (qIsNaN(avgDeltaTime_)) {
    qreal avg = 0;

    if (!isEmpty()) {
      for (auto& dataset: *this)
        avg += dataset->avgDeltaTime();

      avg /= super::count();
    }

    avgDeltaTime_ = avg;
  }

  return avgDeltaTime_;
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

Curve::rc Datasets::makeAvgCurve(core::Session::rc session, bool trans, bool cut) const {
  if (!avgCurve_.isEmpty())
    return avgCurve_;

  TakesLongTime __;

  Curve res;

  for (auto& dataset: *this) {
    shp_DatasetLens lens = session.datasetLens(*dataset, *this, session.norm(), trans, cut);
    Curve single = lens->makeCurve();
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
