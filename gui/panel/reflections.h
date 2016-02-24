/** \file
 */

#ifndef REFLECTIONS_H
#define REFLECTIONS_H

#include "panel.h"

namespace panel {
//------------------------------------------------------------------------------

class ReflectionView: public ListView {
  SUPER(ReflectionView,ListView) Q_OBJECT
public:
  ReflectionView();
};

class Reflections: public BoxPanel {
  SUPER(Reflections,BoxPanel)
public:
  Reflections(MainWin&,Session&);

private:
  ReflectionView *reflectionView;
};

//------------------------------------------------------------------------------
}
#endif
