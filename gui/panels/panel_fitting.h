// ************************************************************************** //
//
//  STeCa2:    StressTextureCalculator ver. 2
//
//! @file      panel_fitting.h
//! @brief     Fitting panel.
//!
//! @homepage  http://apps.jcns.fz-juelich.de/steca2
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016
//! @authors   Scientific Computing Group at MLZ Garching
//! @authors   Rebecca Brydon, Jan Burle,  Antti Soininen
//! @authors   Based on the original STeCa by Christian Randau
//
// ************************************************************************** //

#ifndef PANEL_FITTING_H
#define PANEL_FITTING_H

#include "panel.h"
#include "views.h"

namespace gui { namespace panel {
//------------------------------------------------------------------------------

class Fitting : public TabsPanel {
  SUPER(Fitting, TabsPanel)
public:
  Fitting(TheHub&);

private:
  class ReflectionView *reflectionView_;

  QSpinBox       *spinDegree_;
  QComboBox      *comboReflType_;
  QDoubleSpinBox *spinRangeMin_,   *spinRangeMax_;
  QDoubleSpinBox *spinGuessPeakX_, *spinGuessPeakY_, *spinGuessFWHM_;
  QLineEdit      *readFitPeakX_,   *readFitPeakY_,   *readFitFWHM_;
  bool            silentSpin_;

  void setReflControls(calc::shp_Reflection);
};

//------------------------------------------------------------------------------
}}
#endif // PANEL_FITTING_H
