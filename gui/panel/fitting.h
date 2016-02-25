/** \file
 */

#ifndef FITTING_H
#define FITTING_H

#include "panel.h"

namespace panel {
//------------------------------------------------------------------------------

class ReflectionView: public ListView {
  SUPER(ReflectionView,ListView) Q_OBJECT
public:
  ReflectionView();
};

//------------------------------------------------------------------------------

class Fitting: public BoxPanel {
  SUPER(Fitting,BoxPanel)
public:
  Fitting(MainWin&,Session&);

private:
  QSpinBox *spinDegree;
  ReflectionView *reflectionView;
};

//------------------------------------------------------------------------------
}
#endif
