#include "core_dataset.h"

#include "core_curve.h"
#include "core_fit_fitting.h"
#include "core_session.h"

#include <Eigen/Core>

namespace core {
//------------------------------------------------------------------------------

QVector<str> const Dataset::attributeTags = {
  "date", "comment",
  "X", "Y", "Z", "ω", "2θ", "φ", "χ",
  "PST", "SST", "ΩM", "monitor", "Δt",
};

rcstr Dataset::getAttributeTag(int i) {
  return attributeTags.at(i);
}

Dataset::Dataset(
  rcstr date_, rcstr comment_,
  qreal motorXT_, qreal motorYT_, qreal motorZT_,
  qreal motorOmg_, qreal motorTth_, qreal motorPhi_, qreal motorChi_,
  qreal motorPST_, qreal motorSST_, qreal motorOMGM_,
  qreal mon_, qreal deltaTime_,
  QSize const& size_, intens_t const* intensities_)

: file(nullptr)
, date(date_), comment(comment_)
, motorXT(motorXT_), motorYT(motorYT_), motorZT(motorZT_), motorOmg(motorOmg_), motorTth(motorTth_)
, motorPhi(motorPhi_), motorChi(motorChi_), motorPST(motorPST_), motorSST(motorSST_), motorOMGM(motorOMGM_)
, mon(mon_), deltaTime(deltaTime_)
, image(size_,intensities_) {
}

qreal Dataset::getNumericalAttributeValue(eAttributes const tag) const {
  switch (tag) {
  case MOTOR_X:       return motorXT;
  case MOTOR_Y:       return motorYT;
  case MOTOR_Z:       return motorZT;
  case MOTOR_OMG:     return motorOmg;
  case MOTOR_TTH:     return motorTth;
  case MOTOR_PHI:     return motorPhi;
  case MOTOR_CHI:     return motorChi;
  case MOTOR_PST:     return motorPST;
  case MOTOR_SST:     return motorSST;
  case MOTOR_OMGM:    return motorOMGM;
  case MON:           return mon;
  case DELTA_TIME:    return deltaTime;
  default: NEVER_HERE return qQNaN();
  }
}

str Dataset::getAttributeStrValue(int e) const {
  qreal value = 0;

  switch (e) {
  case DATE:        return date;
  case COMMENT:     return comment;

  case MOTOR_X:     value = motorXT;    break;
  case MOTOR_Y:     value = motorYT;    break;
  case MOTOR_Z:     value = motorZT;    break;
  case MOTOR_OMG:   value = motorOmg;   break;
  case MOTOR_TTH:   value = motorTth;   break;
  case MOTOR_PHI:   value = motorPhi;   break;
  case MOTOR_CHI:   value = motorChi;   break;
  case MOTOR_PST:   value = motorPST;   break;
  case MOTOR_SST:   value = motorSST;   break;
  case MOTOR_OMGM:  value = motorOMGM;  break;
  case MON:         value = mon;        break;
  case DELTA_TIME:  value = deltaTime;  break;
  }

  return QString().setNum(value);
}

File const& Dataset::getFile() const {
  ASSERT(file)
      return *file;
}

Range Dataset::getRgeIntens(bool global) const {
  return global ? getFile().getRgeIntens() : getImage().getRgeIntens();
}

void Dataset::addIntensities(Dataset const& that) {
  ASSERT(image.getCount() == that.image.getCount())
  image.addIntensities(that.image.getIntensities());
}

using matrix3d = Eigen::Matrix<qreal,3,3>;
using vector3d = Eigen::Matrix<qreal,3,1>;

// Returns a clockwise rotation matrix around the x axis.
matrix3d rotationCWx(const qreal angle) {
  matrix3d m;
  m <<  1,       0,                0
       ,0, std::cos(angle), -std::sin(angle)
       ,0, std::sin(angle),  std::cos(angle);
  return m;
}

// Returns a clockwise rotation matrix around the z axis.
matrix3d rotationCWz(const qreal angle) {
  matrix3d m;
  m <<   std::cos(angle), -std::sin(angle), 0
        ,std::sin(angle),  std::cos(angle), 0
        ,      0        ,        0        , 1;
  return m;
}

// Returns a counterclockwise rotation matrix around the z axis.
matrix3d rotationCCWz(const qreal angle) {
    return rotationCWz(angle).transpose();
}

// Calculates the polefigure coordinates alpha and beta with regards to
// sample orientation and diffraction angles.
void calculateAlphaBeta(const qreal omgDet, const qreal phiDet,
                        const qreal chiDet,
                        const qreal tthRef, const qreal gammaRef,
                        qreal& alpha, qreal& beta) {
  // Note that the rotations here do not correspond to C. Randau's dissertation.
  // The rotations given in [J. Appl. Cryst. (2012) 44, 641-644] are incorrect.
  const vector3d rotated =   rotationCWz (phiDet)
                           * rotationCWx (chiDet)
                           * rotationCWz (omgDet)
                           * rotationCWx (gammaRef)
                           * rotationCCWz(tthRef / 2)
                           * vector3d(0,1,0);
  alpha = std::acos(rotated(2,0));
  beta  = std::atan2(rotated(0,0), rotated(1,0));
  // Mirror angles.
  if (alpha > M_PI / 2) {
    alpha = std::abs(alpha - M_PI);
    beta += beta < 0 ? M_PI : -M_PI;
  }
  // Keep beta between 0 and 2pi.
  if (beta < 0)
    beta += 2 * M_PI;
}

ReflectionInfo Dataset::makeReflectionInfo(Session & session,
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
  gammaCutCurve.subtractFunction(sectorBg);
  auto peakFunction = reflection.makePeakFunction();
  fit::fitPeak(*peakFunction,
               gammaCutCurve,
               reflection.getRange());
  qreal alpha;
  qreal beta;
  calculateAlphaBeta(
    getNumericalAttributeValue(Dataset::MOTOR_OMG),
    getNumericalAttributeValue(Dataset::MOTOR_PHI),
    getNumericalAttributeValue(Dataset::MOTOR_CHI),
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
