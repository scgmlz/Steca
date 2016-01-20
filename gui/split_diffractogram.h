#ifndef SPLIT_DIFFRACTOGRAM_H
#define SPLIT_DIFFRACTOGRAM_H

#include "split.h"

class SplitDiffractogram: public Split {
  SUPER(SplitDiffractogram,Split)
public:
  SplitDiffractogram(MainWin&,Session&);
};

#endif
