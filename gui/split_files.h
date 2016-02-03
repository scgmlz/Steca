/** \file
 */

#ifndef SPLIT_FILES_H
#define SPLIT_FILES_H

#include "gui_helpers.h"

class MainWin; class Session;

class SplitFiles: public BoxWidget {
  SUPER(SplitFiles,BoxWidget)
public:
  SplitFiles(MainWin&,Session&);
};

#endif
