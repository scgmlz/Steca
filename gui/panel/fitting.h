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

class Fitting;

class ReflectionView: public TreeListView {
  SUPER(ReflectionView,TreeListView)
public:
  using Model = model::ReflectionViewModel;

  ReflectionView(Fitting&,Model&);

  void addReflection();
  void removeSelected();
  void update();

protected:
  void selectionChanged(QItemSelection const&, QItemSelection const&);

private:
  Fitting &fitting;
  Model   &model;
};

//------------------------------------------------------------------------------

class Fitting: public BoxPanel {
  SUPER(Fitting,BoxPanel)
  friend class ReflectionView;
public:
  Fitting(MainWin&,Session&);

private:
  QSpinBox *spinDegree;
  ReflectionView *reflectionView;
};

//------------------------------------------------------------------------------
}
#endif
