/** \file
 */

#ifndef SPLIT_IMAGE_H
#define SPLIT_IMAGE_H

#include "gui_helpers.h"

class MainWin; class Session;

class SplitImage: public BoxWidget {
  SUPER(SplitImage,BoxWidget)
public:
  SplitImage(MainWin&,Session&);
};

#endif
