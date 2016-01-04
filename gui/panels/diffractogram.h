#ifndef DIFFRACTOGRAM_H
#define DIFFRACTOGRAM_H

#include "panel.h"

namespace panel {

class Diffractogram: public Panel {
  SUPER(Diffractogram,Panel)
public:
  Diffractogram(MainWin&);
};

}

#endif
