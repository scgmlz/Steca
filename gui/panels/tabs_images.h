// ************************************************************************** //
//
//  STeCa2:    StressTextureCalculator ver. 2
//
//! @file      tabs_images.h
//!
//! @homepage  http://apps.jcns.fz-juelich.de/steca2
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016
//! @authors   Scientific Computing Group at MLZ Garching
//! @authors   Rebecca Brydon, Jan Burle,  Antti Soininen
//! @authors   Based on the original STeCa by Christian Randau
//
// ************************************************************************** //

#ifndef TABS_IMAGES_H
#define TABS_IMAGES_H

#include "panel.h"

namespace gui { namespace panel {
//------------------------------------------------------------------------------

class TabsImages : public TabsPanel {
  CLS(TabsImages) SUPER(TabsPanel)
public:
  TabsImages(TheHub&);

private:
  QSlider *imageScale_;
};

//------------------------------------------------------------------------------
}}
#endif // TABS_IMAGES_H
