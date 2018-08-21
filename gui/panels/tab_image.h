//  ***********************************************************************************************
//
//  Steca: stress and texture calculator
//
//! @file      gui/panels/tab_image.h
//! @brief     Defines classes DataImageTab, CorrImageTab, and their dependences
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
//  ***********************************************************************************************

#ifndef TAB_IMAGE_H
#define TAB_IMAGE_H

#include "qcr/widgets/views.h"
#include "qcr/engine/cell.h"
#include "core/raw/measurement.h"
#include "core/raw/image.h"

//! Pure virtual base class for DataImageTab and CorrImageTab, shows a detector image and controls.

class ImageTab : public QcrWidget {
public:
    ImageTab();
protected:
    void render();
    virtual QPixmap pixmap() = 0;
    QPixmap blankPixmap();
    QImage makeImage(const Image&);
    class QHBoxLayout* box1_;
    class QVBoxLayout* controls_;
    class ImageView* imageView_;
};

//! A tab for a data image and associated controls.

//! To display a 2d detector image of diffraction data,
//! as opposed to the correction data in CorrImageTab.

class DataImageTab : public ImageTab {
public:
    DataImageTab();
private:
    const Measurement* measurement();
    QPixmap pixmap() final;
    QcrCell<int> iMeas {1};
};

//! A tab for the correction image and associated controls.

//! To display a 2d detector image of correction data,
//! as opposed to the diffraction data in DataImageTab.

class CorrImageTab : public ImageTab {
public:
    CorrImageTab();
private:
    QPixmap pixmap() final;
};

#endif // TAB_IMAGE_H
