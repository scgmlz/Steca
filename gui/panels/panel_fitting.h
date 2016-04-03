/** \file
 */

#ifndef PANEL_FITTING_H
#define PANEL_FITTING_H

#include "panel.h"
#include "core_reflection.h"
#include "models.h"

namespace panel {
//------------------------------------------------------------------------------

class ReflectionView: public HubListView {
  SUPER(ReflectionView,HubListView)
public:
  using Model = models::ReflectionViewModel;

  ReflectionView(TheHub&);

  void addReflection(int type);
  void removeSelected();
  bool hasReflections() const;

  void update();

protected:
  void selectionChanged(QItemSelection const&, QItemSelection const&);

private:
  Model  &model;
};

//------------------------------------------------------------------------------

class Fitting: public BoxPanel {
  SUPER(Fitting,BoxPanel)
public:
  Fitting(TheHub&);

private:
  QSpinBox  *spinDegree;
  QComboBox *comboReflType;
  ReflectionView *reflectionView;
  QDoubleSpinBox *spinRangeMin, *spinRangeMax;
  QDoubleSpinBox *spinGuessPeakX, *spinGuessPeakY, *spinGuessFwhm;
  QLineEdit      *readFitPeakX,   *readFitPeakY,   *readFitFwhm;
  bool silentSpin;

  void setReflControls(core::shp_Reflection const&);
  void updateReflectionControls();
private:
  void enableReflControls(bool); // TODO REVIEW
};

//------------------------------------------------------------------------------
}
#endif
