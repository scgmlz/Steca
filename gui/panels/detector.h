#ifndef DETECTOR_H
#define DETECTOR_H

#include "panel.h"

namespace panel {

class Detector: public BoxPanel {
  SUPER(Detector,BoxPanel)
public:
  Detector(MainWin&);
};

}

#endif
