/** \file
 */

#ifndef FITTING_H
#define FITTING_H

#include "panel.h"

namespace model {
class ReflectionViewModel;
}

namespace panel {
//------------------------------------------------------------------------------

class ReflectionView: public ListView {
  SUPER(ReflectionView,ListView) Q_OBJECT
public:
  using Model = model::ReflectionViewModel;

  ReflectionView(Model&);

  void addReflection();

private:
  Model &model;
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
