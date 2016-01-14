#include "core_dataset.h"

namespace core {

Dataset::Dataset(File& file_,
  rcstr date_, rcstr comment_,
  float motorXT_,  float motorYT_,  float motorZT_, float motorOmg_, float motorTth_,
  float motorPhi_, float motorChi_,
  int   mon_, double deltaTime_,
  float motorPST_, float motorSST_, float motorOMGM_,
  QSize const& size_, int const* intensities_)

: file(file_), date(date_), comment(comment_)
, motorXT(motorXT_), motorYT(motorYT_), motorZT(motorZT_), motorOmg(motorOmg_), motorTth(motorTth_)
, motorPhi(motorPhi_), motorChi(motorChi_), motorPST(motorPST_), motorSST(motorSST_), motorOMGM(motorOMGM_)
, mon(mon_), deltaTime(deltaTime_)
, image(size_,intensities_) {

}

Datasets::Datasets() {
}

}

// eof
