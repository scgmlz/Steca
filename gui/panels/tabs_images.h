// ************************************************************************** //
//
//  Steca2: stress and texture calculator
//
//! @file      gui/panels/tabs_images.h
//! @brief     Defines class TabsImages
//!
//! @homepage  https://github.com/scgmlz/Steca2
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
// ************************************************************************** //

#ifndef TABS_IMAGES_H
#define TABS_IMAGES_H

#include "calc/lens.h"
#include "panel.h"

class Measurement;
class QDoubleSpinBox;
class QSpinBox;

namespace gui {
namespace panel {

class ImageWidget;

class TabsImages : public TabsPanel {
public:
    TabsImages();

private:
    QPixmap makeBlankPixmap();

    QImage makeImage(QSharedPointer<Image>, bool curvedScale);
    QPixmap makePixmap(QSharedPointer<Image>);
    QPixmap makePixmap(Measurement const&, Range const&, Range const&);

    void setSuite(QSharedPointer<Suite>);
    void render();

    QSharedPointer<Suite> dataseq_;
    ImageWidget *dataImageWidget_, *corrImageWidget_;

    QSpinBox* spinN_;
    QSpinBox *numSlices_, *numSlice_, *numBin_;
    QDoubleSpinBox *minGamma_, *maxGamma_;

    QSharedPointer<SequenceLens> lens_;
};

} // namespace panel
} // namespace gui

#endif // TABS_IMAGES_H
