/** \file
 */

#ifndef SPLIT_DATASETS_H
#define SPLIT_DATASETS_H

#include "gui_helpers.h"

class MainWin; class Session;

class SplitDatasets: public BoxWidget {
  SUPER(SplitDatasets,BoxWidget)
public:
  SplitDatasets(MainWin&,Session&);
};

#endif
