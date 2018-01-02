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
public:
    TabsImages();

private:
    QPixmap makeBlankPixmap();

    QImage makeImage(shp_Image, bool curvedScale);
    QPixmap makePixmap(shp_Image);
    QPixmap makePixmap(OneDataset const&, Range const&, Range const&);

    void setDataset(shp_Dataset);
    void render();

    shp_Dataset dataset_;
    ImageWidget *dataImageWidget_, *corrImageWidget_;

    QSpinBox* spinN_;
    QSpinBox *numSlices_, *numSlice_, *numBin_;
    QDoubleSpinBox *minGamma_, *maxGamma_;

    calc::shp_DatasetLens lens_;
};

} // namespace panel
} // namespace gui

#endif // TABS_IMAGES_H
