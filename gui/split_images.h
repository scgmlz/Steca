#ifndef SPLIT_IMAGES_H
#define SPLIT_IMAGES_H

#include "split.h"

class SplitImages: public Split {
  SUPER(SplitImages,Split)
public:
  SplitImages(MainWin&);
};

#endif
