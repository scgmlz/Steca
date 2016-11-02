// ************************************************************************** //
//
//  STeCa2:    StressTextureCalculator ver. 2
//
//! @file      tabs_setup.h
//!
//! @homepage  http://apps.jcns.fz-juelich.de/steca2
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016
//! @authors   Scientific Computing Group at MLZ Garching
//! @authors   Rebecca Brydon, Jan Burle, Antti Soininen
//! @authors   Based on the original STeCa by Christian Randau
//
// ************************************************************************** //

#ifndef TABS_SETUP_H
#define TABS_SETUP_H

#include "panel.h"

namespace gui { namespace panel {
//------------------------------------------------------------------------------

class ReflectionView;

class TabsSetup : public TabsPanel {
  CLS(TabsSetup) SUPER(TabsPanel)
public:
  TabsSetup(TheHub&);

private:
  // geometry
  QDoubleSpinBox *detDistance_, *detPixelSize_;
  QSpinBox       *beamOffsetI_, *beamOffsetJ_;
  QSpinBox       *cutLeft_, *cutTop_, *cutRight_, *cutBottom_;

  void setToHub();
  void setFromHub();

  // background
  QSpinBox       *spinDegree_;

  // reflections
  ReflectionView *reflectionView_;
  QComboBox      *comboReflType_;
  QDoubleSpinBox *spinRangeMin_,   *spinRangeMax_;
  QDoubleSpinBox *spinGuessPeakX_, *spinGuessPeakY_, *spinGuessFWHM_;
  QLineEdit      *readFitPeakX_,   *readFitPeakY_,   *readFitFWHM_;

  bool silentSpin_ = false;
};

//------------------------------------------------------------------------------
}}
#endif // TABS_SETUP_H
