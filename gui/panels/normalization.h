#ifndef NORMALIZATION_H
#define NORMALIZATION_H

#include "panel.h"

namespace panel {

class Normalization: public BoxPanel {
  SUPER(Normalization,BoxPanel)
public:
  Normalization(MainWin&,Session&);
};

}

#endif
