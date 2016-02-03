/** \file
 */

#ifndef SPLIT_DIFFRACTOGRAM_H
#define SPLIT_DIFFRACTOGRAM_H

#include "gui_helpers.h"

class MainWin; class Session;

class SplitDiffractogram: public BoxWidget {
  SUPER(SplitDiffractogram,BoxWidget)
public:
  SplitDiffractogram(MainWin&,Session&);
};

#endif
