// ************************************************************************** //
//
//  STeCa2:    StressTexCalculator ver. 2
//
//! @file      panel_fitting.h
//! @brief     Fitting panel.
//!
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016
//! @authors   Scientific Computing Group at MLZ Garching
//! @authors   Original version: Christian Randau
//! @authors   Version 2: Antti Soininen, Jan Burle, Rebecca Brydon
//
// ************************************************************************** //

#ifndef PANEL_FITTING_H
#define PANEL_FITTING_H

#include "panel.h"
#include "core_reflection.h"
#include "models.h"

namespace gui { namespace panel {
//------------------------------------------------------------------------------

class ReflectionView: public ListView {
  SUPER(ReflectionView,ListView)
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
  Model  &_model;
};

//------------------------------------------------------------------------------

class Fitting: public TabsPanel {
  SUPER(Fitting,TabsPanel)
public:
  Fitting(TheHub&);

private:
  QSpinBox  *spinDegree_;
  QComboBox *comboReflType_;
  ReflectionView *reflectionView_;
  QDoubleSpinBox *spinRangeMin_, *spinRangeMax_;
  QDoubleSpinBox *spinGuessPeakX_, *spinGuessPeakY_, *spinGuessFWHM_;
  QLineEdit      *readFitPeakX_,   *readFitPeakY_,   *readFitFWHM_;
  bool silentSpin_;

  void setReflControls(core::shp_Reflection const&);
  void updateReflectionControls();

private:
  void enableReflControls(bool); // TODO REVIEW
};

//------------------------------------------------------------------------------
}}
#endif // PANEL_FITTING_H
