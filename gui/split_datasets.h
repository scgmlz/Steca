#ifndef SPLIT_DATASETS_H
#define SPLIT_DATASETS_H

#include "split.h"

class SplitDatasets: public Split {
  SUPER(SplitDatasets,Split)
public:
  SplitDatasets(MainWin&);
};

#endif
