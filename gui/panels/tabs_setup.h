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
//! @authors   Rebecca Brydon, Jan Burle,  Antti Soininen
//! @authors   Based on the original STeCa by Christian Randau
//
// ************************************************************************** //

#ifndef TABS_SETUP_H
#define TABS_SETUP_H

#include "panel.h"

namespace gui { namespace panel {
//------------------------------------------------------------------------------

class TabsSetup : public TabsPanel {
  CLS(TabsSetup) SUPER(TabsPanel)
public:
  TabsSetup(TheHub&);

private:
  QDoubleSpinBox *detDistance_, *detPixelSize_;
  QSpinBox       *beamOffsetI_, *beamOffsetJ_;

  void setToHub();
  void setFromHub();

  QSpinBox       *marginLeft_, *marginTop_, *marginRight_, *marginBottom_;
};

//------------------------------------------------------------------------------
}}
#endif // TABS_SETUP_H
