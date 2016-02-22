/** \file
 */

#ifndef BACKGROUND_H
#define BACKGROUND_H

#include "panel.h"

namespace panel {

class Background: public BoxPanel {
  SUPER(Background,BoxPanel) Q_OBJECT
public:
  Background(MainWin&,Session&);

private:
  QSpinBox *spinDegree;
};

}

#endif
