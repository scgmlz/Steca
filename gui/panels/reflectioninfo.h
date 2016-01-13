#ifndef REFLECTIONINFO_H
#define REFLECTIONINFO_H

#include "panel.h"

namespace panel {

class ReflectionInfo: public BoxPanel {
  SUPER(ReflectionInfo,BoxPanel)
public:
  ReflectionInfo(MainWin&);
};

}

#endif
