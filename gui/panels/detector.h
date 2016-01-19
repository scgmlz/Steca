#ifndef DETECTOR_H
#define DETECTOR_H

#include "panel.h"

class Session;

namespace panel {

class Detector: public GridPanel {
  SUPER(Detector,GridPanel)
public:
  Detector(MainWin&);
 ~Detector();

private:
  QDoubleSpinBox *spinDistance, *spinPixelSize;
  QCheckBox      *checkIsBeamOffset;
  QSpinBox       *spinOffsetX, *spinOffsetY;

  bool isSetting;
  void setTo(Session&);
  void setFrom(Session&);

  void readSettings(Session&);
  void saveSettings();
};

}

#endif
