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

class ReflectionView: public TreeListView {
  SUPER(ReflectionView,TreeListView)
public:
  using Model = model::ReflectionViewModel;

  ReflectionView(TheHub&);

  void addReflection();
  void removeSelected();
  void update();

protected:
  void selectionChanged(QItemSelection const&, QItemSelection const&);

private:
  TheHub &theHub;
  Model  &model;
};

//------------------------------------------------------------------------------

class Fitting: public BoxPanel {
  SUPER(Fitting,BoxPanel)
  friend class ReflectionView;
public:
  Fitting(TheHub&);

private:
  QSpinBox *spinDegree;
  ReflectionView *reflectionView;
};

//------------------------------------------------------------------------------
}
#endif
