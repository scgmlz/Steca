// ************************************************************************** //
//
//  Steca2: stress and texture calculator
//
//! @file      gui/panels/tabs_images.h
//! @brief     Defines ...
//!
//! @homepage  https://github.com/scgmlz/Steca2
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2017
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
// ************************************************************************** //

#ifndef TABS_IMAGES_H
#define TABS_IMAGES_H

#include "calc/calc_lens.h"
#include "panel.h"

namespace gui {
namespace panel {

class ImageWidget;

class TabsImages : public TabsPanel {
    CLASS(TabsImages) SUPER(TabsPanel) public : TabsImages(TheHub&);

private:
    QPixmap makeBlankPixmap();

    QImage makeImage(typ::shp_Image, bool curvedScale);
    QPixmap makePixmap(typ::shp_Image);
    QPixmap makePixmap(data::OneDataset::rc, gma_rge::rc, tth_rge::rc);

    void setDataset(data::shp_Dataset);
    void render();

    data::shp_Dataset dataset_;
    ImageWidget *dataImageWidget_, *corrImageWidget_;

    QSpinBox* spinN_;
    QSpinBox *numSlices_, *numSlice_, *numBin_;
    QDoubleSpinBox *minGamma_, *maxGamma_;

    calc::shp_DatasetLens lens_;
};


}
}
#endif // TABS_IMAGES_H
