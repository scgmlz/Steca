// ************************************************************************** //
//
// STeCa2 - StressTextureCalculator ver. 2
//
// Copyright (C) 2016 Forschungszentrum Jülich GmbH 2016
//
// This program is free software: you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the Free
// Software Foundation, either version 3 of the License, or (at your option)
// any later version.
//
// This program is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
// or FITNESS FOR A PARTICULAR PURPOSE.
//
// See the COPYING and AUTHORS files for more details.
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
