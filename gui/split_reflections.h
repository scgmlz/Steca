/** \file
 */

#ifndef SPLIT_REFLECTIONS_H
#define SPLIT_REFLECTIONS_H

#include "gui_helpers.h"

class MainWin; class Session;

//------------------------------------------------------------------------------

class SplitReflections: public BoxWidget {
  SUPER(SplitReflections,BoxWidget)
public:
  SplitReflections(MainWin&,Session&);
};

//------------------------------------------------------------------------------
#endif
