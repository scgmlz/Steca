#ifndef SPLIT_IMAGE_H
#define SPLIT_IMAGE_H

#include "split.h"

class SplitImage: public Split {
  SUPER(SplitImage,Split)
public:
  SplitImage(MainWin&);
};

#endif
