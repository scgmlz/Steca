// ************************************************************************** //
//
//  Steca: stress and texture calculator
//
//! @file      gui/panels/subframe_image.h
//! @brief     Defines class SubframeImage
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
// ************************************************************************** //

#ifndef SUBFRAME_IMAGE_H
#define SUBFRAME_IMAGE_H

#include "core/calc/lens.h"
#include "core/data/cluster.h"
#include "gui/base/new_q.h"

//! Part of the main window that shows a detector image, with associated controls.

//! There are two tabs: data image and correction data image.

class SubframeImage final : public QTabWidget {
public:
    SubframeImage();

private:
    QPixmap makeBlankPixmap();

    QImage makeImage(shp_Image, bool curvedScale);
    QPixmap makePixmap(shp_Image);
    QPixmap makePixmap(class Measurement const&, const Range&, const Range&);

    void onHighlight();
    void render();

    const Cluster* cluster_ {nullptr};
    class ImageWidget *dataImageWidget_, *corrImageWidget_;

    QSpinBox* spinN_;
    QSpinBox *numSlices_, *numSlice_, *numBin_;
    QDoubleSpinBox *minGamma_, *maxGamma_;

    shp_SequenceLens lens_;
};

#endif // SUBFRAME_IMAGE_H
