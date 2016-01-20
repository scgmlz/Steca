#ifndef SPLIT_FILES_H
#define SPLIT_FILES_H

#include "split.h"

class SplitFiles: public Split {
  SUPER(SplitFiles,Split)
public:
  SplitFiles(MainWin&,Session&);
};

#endif
