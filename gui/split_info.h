#ifndef SPLIT_INFO_H
#define SPLIT_INFO_H

#include "split.h"

class SplitInfo: public Split {
  SUPER(SplitInfo,Split)
public:
  SplitInfo(MainWin&);
};

#endif
