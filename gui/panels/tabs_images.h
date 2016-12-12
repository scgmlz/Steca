/*******************************************************************************
 * REVIEW: STeCa2 - StressTextureCalculator ver. 2
 *
 * Copyright (C) 2016 Forschungszentrum Jülich GmbH 2016
 *
 * This program is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the Free
 * Software Foundation, either version 3 of the License, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE.
 *
 * See the COPYING and AUTHORS files for more details.
 ******************************************************************************/

#ifndef TABS_IMAGES_H
#define TABS_IMAGES_H

#include "panel.h"
#include "core/calc/calc_lens.h"

namespace gui { namespace panel {
//------------------------------------------------------------------------------

class ImageWidget;

class TabsImages : public TabsPanel {
  CLS(TabsImages) SUPER(TabsPanel)
public:
  TabsImages(TheHub&);

private:
  QPixmap makeBlankPixmap();

  QImage  makeImage(typ::Image::rc, bool curvedScale);
  QPixmap makePixmap(typ::Image::rc);
  QPixmap makePixmap(data::OneDataset::rc, gma_rge::rc, tth_rge::rc);

  void setDataset(data::shp_Dataset);
  void render();

  data::shp_Dataset dataset_;
  ImageWidget *dataImageWidget_, *corrImageWidget_;

  QSpinBox *spinN_;
  QSpinBox *numSlices_, *numSlice_, *numBin_;
  QDoubleSpinBox *minGamma_, *maxGamma_;

  calc::shp_DatasetLens lens_;
};

//------------------------------------------------------------------------------
}}
#endif // TABS_IMAGES_H
