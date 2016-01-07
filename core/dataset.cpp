#include "dataset.h"

namespace core {

Dataset::Dataset(QSize const& size_, int const* intensities_,
  rcstr comment_,
  float motorXT_,  float motorYT_,  float motorZT_, float motorOmg_, float motorTth_,
  float motorPhi_, float motorChi_,
  double mon_, double deltaTime_,
  float motorPST_, float motorSST_, float motorOMGM_)

: comment(comment_), intensities(size_,intensities_)
, motorXT(motorXT_), motorYT(motorYT_), motorZT(motorZT_), motorOmg(motorOmg_), motorTth(motorTth_)
, motorPhi(motorPhi_), motorChi(motorChi_), motorPST(motorPST_), motorSST(motorSST_), motorOMGM(motorOMGM_)
, mon(mon_), deltaTime(deltaTime_) {

}

Datasets::Datasets() {
}

Datasets::~Datasets() {
  for (auto dataset: *this)
    delete dataset;
}

}

// eof
