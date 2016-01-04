#ifndef REFLECTIONINFO_H
#define REFLECTIONINFO_H

#include "panel.h"

namespace panel {

class ReflectionInfo: public Panel {
  SUPER(ReflectionInfo,Panel)
public:
  ReflectionInfo(MainWin&);
};

}

#endif
