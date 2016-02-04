/** \file
 */

#ifndef DETECTOR_H
#define DETECTOR_H

#include "panel.h"

namespace panel {

class Detector: public GridPanel {
  SUPER(Detector,GridPanel)
public:
  Detector(MainWin&,Session&);
 ~Detector();

private:
  QDoubleSpinBox *spinDistance, *spinPixelSize;
  QCheckBox      *checkIsBeamOffset;
  QSpinBox       *spinOffsetX, *spinOffsetY;

  void setTo(Session&);
  void setFrom(Session&);

  void readSettings(Session&);
  void saveSettings();
};

}

#endif
