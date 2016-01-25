#include "core_dataset.h"

namespace core {

Dataset::Dataset(Datasets& datasets_,
  rcstr date_, rcstr comment_,
  qreal motorXT_, qreal motorYT_, qreal motorZT_,
  qreal motorOmg_, qreal motorTth_, qreal motorPhi_, qreal motorChi_,
  qreal motorPST_, qreal motorSST_, qreal motorOMGM_,
  qreal mon_, qreal deltaTime_,
  uint size_, Image::intensity_t const* intensities_)

: datasets(datasets_), date(date_), comment(comment_)
, motorXT(motorXT_), motorYT(motorYT_), motorZT(motorZT_), motorOmg(motorOmg_), motorTth(motorTth_)
, motorPhi(motorPhi_), motorChi(motorChi_), motorPST(motorPST_), motorSST(motorSST_), motorOMGM(motorOMGM_)
, mon(mon_), deltaTime(deltaTime_)
, image(size_,intensities_) {
}

str Dataset::getAttributeStrValue(int e) const {
  qreal value;

  switch (e) {
  case Datasets::DATE:
    return date;
  case Datasets::COMMENT:
    return comment;

  case Datasets::MOTOR_X:
    value = motorXT;    break;
  case Datasets::MOTOR_Y:
    value = motorYT;    break;
  case Datasets::MOTOR_Z:
    value = motorZT;    break;
  case Datasets::MOTOR_OMG:
    value = motorOmg;   break;
  case Datasets::MOTOR_TTH:
    value = motorTth;   break;
  case Datasets::MOTOR_PHI:
    value = motorPhi;   break;
  case Datasets::MOTOR_CHI:
    value = motorChi;   break;
  case Datasets::MOTOR_PST:
    value = motorPST;   break;
  case Datasets::MOTOR_SST:
    value = motorSST;   break;
  case Datasets::MOTOR_OMGM:
    value = motorOMGM;  break;
  case Datasets::MON:
    value = mon;        break;
  case Datasets::DELTA_TIME:
    value = deltaTime;  break;
  case Datasets::NUM_ATTRIBUTES:
    NOT_HERE
  }
  return QString().setNum(value);
}

Datasets::Datasets() {
}

QVector<str> const Datasets::attributeTags = {
  "date", "comment",
  "X", "Y", "Z", "ω", "2θ", "φ", "χ",
  "PST", "SST", "ΩM", "monitor", "Δt",
};

uint Datasets::getImageSize() const {
  uint size = 0;
  for (auto dataset: *this) {
    uint imageSize = dataset->getImage().getSize();
    if (0 == size)
      size = imageSize;
    else if (size != imageSize)
      return 0;
  }
  return size;
}

Image::intensity_t Datasets::getMaximumIntensity() const {
  Image::intensity_t max = 0;
  for (auto dataset: *this)
    max = qMax(max,dataset->getImage().maximumIntensity());
  return max;
}

}

// eof
