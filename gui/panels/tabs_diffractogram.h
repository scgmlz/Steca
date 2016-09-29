// ************************************************************************** //
//
//  STeCa2:    StressTextureCalculator ver. 2
//
//! @file      tabs_diffractogram.h
//!
//! @homepage  http://apps.jcns.fz-juelich.de/steca2
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016
//! @authors   Scientific Computing Group at MLZ Garching
//! @authors   Rebecca Brydon, Jan Burle,  Antti Soininen
//! @authors   Based on the original STeCa by Christian Randau
//
// ************************************************************************** //

#ifndef TABS_DIFFRACTOGRAM_H
#define TABS_DIFFRACTOGRAM_H

#include "panel.h"

namespace gui { namespace panel {
//------------------------------------------------------------------------------

class TabsDiffractogram : public TabsPanel {
  CLS(TabsDiffractogram) SUPER(TabsPanel)
public:
  TabsDiffractogram(TheHub&);
};

//------------------------------------------------------------------------------
}}
#endif // TABS_DIFFRACTOGRAM_H
