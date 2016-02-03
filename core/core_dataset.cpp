#include "core_dataset.h"

namespace core {

QVector<str> const Dataset::attributeTags = {
  "date", "comment",
  "X", "Y", "Z", "ω", "2θ", "φ", "χ",
  "PST", "SST", "ΩM", "monitor", "Δt",
};

rcstr Dataset::getAttributeTag(int i) {
  return attributeTags.at(i);
}

Dataset::Dataset(File const& file_,
  rcstr date_, rcstr comment_,
  qreal motorXT_, qreal motorYT_, qreal motorZT_,
  qreal motorOmg_, qreal motorTth_, qreal motorPhi_, qreal motorChi_,
  qreal motorPST_, qreal motorSST_, qreal motorOMGM_,
  qreal mon_, qreal deltaTime_,
  QSize const& size_, intens_t const* intensities_)

: file(file_)
, date(date_), comment(comment_)
, motorXT(motorXT_), motorYT(motorYT_), motorZT(motorZT_), motorOmg(motorOmg_), motorTth(motorTth_)
, motorPhi(motorPhi_), motorChi(motorChi_), motorPST(motorPST_), motorSST(motorSST_), motorOMGM(motorOMGM_)
, mon(mon_), deltaTime(deltaTime_)
, image(size_,intensities_) {
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

void Dataset::addIntensities(Dataset const& that) {
  ASSERT(image.getCount() == that.image.getCount())
  image.addIntensities(that.image.getIntensities());
}

}

// eof
