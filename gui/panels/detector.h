#ifndef DETECTOR_H
#define DETECTOR_H

#include "panel.h"

namespace panel {

class Detector: public Panel {
  SUPER(Detector,Panel)
public:
  Detector(MainWin&);
};

}

#endif
