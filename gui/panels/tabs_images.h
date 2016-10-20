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
#include "calc/calc_lens.h"

namespace gui { namespace panel {
//------------------------------------------------------------------------------

class ImageWidget;

class TabsImages : public TabsPanel {
  CLS(TabsImages) SUPER(TabsPanel)
public:
  TabsImages(TheHub&);

private:
  QPixmap makeBlankPixmap();

  QImage  makeImage(typ::Image::rc);
  QPixmap makePixmap(typ::Image::rc);
  QPixmap makePixmap(data::OneDataset::rc, gma_rge::rc);

  void setDataset(data::shp_Dataset);
  void render();

  data::shp_Dataset dataset_;
  ImageWidget *dataImageWidget_, *corrImageWidget_;

  uint n = 0, by = 0;
  QSpinBox *spinN_;

  calc::shp_DatasetLens lens_;
  typ::Range rgeGma_;

  QDoubleSpinBox *minGamma_, *maxGamma_;
};

//------------------------------------------------------------------------------
}}
#endif // TABS_IMAGES_H
