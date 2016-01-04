#ifndef SPLIT_REFLECTIONS_H
#define SPLIT_REFLECTIONS_H

#include "split.h"

class SplitReflections: public Split {
  SUPER(SplitReflections,Split)
public:
  SplitReflections(MainWin&);
};

#endif
