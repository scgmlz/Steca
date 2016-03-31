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

#include "core_curve.h"
#include "core_fit_fitting.h"
#include "core_session.h"
#include "core_reflection_info.h"
#include <cmath>

namespace core {
//------------------------------------------------------------------------------

// datase attributes
enum {
  attrDATE, attrCOMMENT,

  attrMOTOR_XT, attrMOTOR_YT, attrMOTOR_ZT,
  attrMOTOR_OMG, attrMOTOR_TTH, attrMOTOR_PHI, attrMOTOR_CHI,
  attrMOTOR_PST, attrMOTOR_SST, attrMOTOR_OMGM,
  attrDELTA_MON_COUNT, attrDELTA_TIME,

  NUM_ATTRIBUTES
};

static QVector<str> const attributeTags = {
  "date", "comment",
  "X", "Y", "Z",
  "ω", "2θ", "φ", "χ",
  "PST", "SST", "ΩM",
  "Δmon", "Δt",
};

uint Dataset::numAttributes() {
  return NUM_ATTRIBUTES;
}

rcstr Dataset::getAttributeTag(uint i) {
  return attributeTags.at(i);
}

Dataset::Dataset(
  File const* file_,
  rcstr date_, rcstr comment_,
  qreal motorXT_, qreal motorYT_, qreal motorZT_,
  qreal motorOmg_, qreal motorTth_, qreal motorPhi_, qreal motorChi_,
  qreal motorPST_, qreal motorSST_, qreal motorOMGM_,
  qreal monCount_, qreal dTime_,
  QSize const& size_, intens_t const* intensities_)

: file(file_)
, date(date_), comment(comment_)
, motorXT(motorXT_), motorYT(motorYT_), motorZT(motorZT_)
, motorOmg(motorOmg_), motorTth(motorTth_), motorPhi(motorPhi_), motorChi(motorChi_)
, motorPST(motorPST_), motorSST(motorSST_), motorOMGM(motorOMGM_)
, monCount(monCount_), dTime(dTime_)
, image(size_,intensities_) {
}

str Dataset::getAttributeStrValue(uint i) const {
  qreal value = 0;

  switch (i) {
  case attrDATE:            return date;
  case attrCOMMENT:         return comment;

  case attrMOTOR_XT:        value = motorXT;    break;
  case attrMOTOR_YT:        value = motorYT;    break;
  case attrMOTOR_ZT:        value = motorZT;    break;
  case attrMOTOR_OMG:       value = motorOmg;   break;
  case attrMOTOR_TTH:       value = motorTth;   break;
  case attrMOTOR_PHI:       value = motorPhi;   break;
  case attrMOTOR_CHI:       value = motorChi;   break;
  case attrMOTOR_PST:       value = motorPST;   break;
  case attrMOTOR_SST:       value = motorSST;   break;
  case attrMOTOR_OMGM:      value = motorOMGM;  break;
  case attrDELTA_MON_COUNT: value = monCount;   break;
  case attrDELTA_TIME:      value = dTime;      break;
  }

  return QString().setNum(value);
}

qreal Dataset::middleTth() const {
  return motorTth;
}

qreal Dataset::monitorCount() const {
  return monCount;
}

qreal Dataset::deltaTime() const {
  return dTime;
}

File const& Dataset::parentFile() const {
  ASSERT(file)
  return *file;
}

Range Dataset::intensRange(bool global) const {
  return global ? parentFile().intensRange() : image.intensRange();
}

void Dataset::addIntensities(Dataset const& that) {
  ASSERT(image.getCount() == that.image.getCount())
  image.addIntensities(that.image.getIntensities());
}

// Returns a clockwise rotation matrix around the x axis.
matrix3d rotationCWx(qreal angle) {
  return matrix3d(1, 0,           0,
                  0, cos(angle), -sin(angle),
                  0, sin(angle),  cos(angle));
}

// Returns a clockwise rotation matrix around the z axis.
matrix3d rotationCWz(qreal angle) {
  return matrix3d(cos(angle), -sin(angle), 0,
                  sin(angle),  cos(angle), 0,
                  0,           0,          1);
}

// Returns a counterclockwise rotation matrix around the z axis.
matrix3d rotationCCWz(qreal angle) {
  matrix3d m = rotationCWz(angle);
  m.transpose();
  return m;
}

// Calculates the polefigure coordinates alpha and beta with regards to
// sample orientation and diffraction angles.
void calculateAlphaBeta(qreal omgDet, qreal phiDet, qreal chiDet, qreal tthRef, qreal gammaRef,
                        qreal& alpha, qreal& beta) {

  omgDet   = degToRad(omgDet);
  phiDet   = degToRad(phiDet);
  chiDet   = degToRad(chiDet);
  tthRef   = degToRad(tthRef);
  gammaRef = degToRad(gammaRef);

  // Note that the rotations here do not correspond to C. Randau's dissertation.
  // The rotations given in [J. Appl. Cryst. (2012) 44, 641-644] are incorrect.
  const vector3d rotated =   rotationCWz (phiDet)
                           * rotationCWx (chiDet)
                           * rotationCWz (omgDet)
                           * rotationCWx (gammaRef)
                           * rotationCCWz(tthRef / 2)
                           * vector3d(0,1,0);
  alpha = std::acos(rotated._2);
  beta  = std::atan2(rotated._0, rotated._1);
  // Mirror angles.
  if (alpha > M_PI / 2) {
    alpha = std::abs(alpha - M_PI);
    beta += beta < 0 ? M_PI : -M_PI;
  }
  // Keep beta between 0 and 2pi.
  if (beta < 0)
    beta += 2 * M_PI;

  alpha = radToDeg(alpha);
  beta  = radToDeg(beta);
}

ReflectionInfo Dataset::makeReflectionInfo(Session const& session,
                                           Reflection const& reflection,
                                           Range const& gammaSector) const {
  auto lenses = session.allLenses(*this, false);
  auto gammaCutCurve = makeCurve(lenses,
                                 gammaSector,
                                 session.getCut().tth_regular);
  const fit::Polynomial sectorBg
    = fit::fitBackground(gammaCutCurve,
                         session.getBgRanges(),
                         session.getBgPolynomialDegree());
  gammaCutCurve = gammaCutCurve.subtract(sectorBg);
  auto peakFunction = reflection.makePeakFunction();
  fit::fitPeak(*peakFunction,
               gammaCutCurve,
               reflection.getRange());
  qreal alpha;
  qreal beta;
  calculateAlphaBeta(motorOmg, motorPhi, motorChi,
    reflection.getRange().center(),
    gammaSector.center(),
    alpha,
    beta
  );
  return ReflectionInfo(alpha,
                        beta,
                        gammaSector,
                        peakFunction->getFitPeak(),
                        peakFunction->getFitFWHM());
}

//------------------------------------------------------------------------------
}
// eof
