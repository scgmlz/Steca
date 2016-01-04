#ifndef NORMALIZATION_H
#define NORMALIZATION_H

#include "panel.h"

namespace panel {

class Normalization: public Panel {
  SUPER(Normalization,Panel)
public:
  Normalization(MainWin&);
};

}

#endif
