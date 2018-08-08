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

#include "qcr/widgets/controls.h"
#include "qcr/widgets/views.h"
#include "qcr/engine/cell.h"
#include "core/raw/image.h"
#include <QPainter>

//! Displays a 2d detector image, and possibly some overlay. Used in ImageTab.

class ImageView final : public QWidget {
public:
    ImageView();
    void setPixmap(const QPixmap&);
    void setScale();
private:
    void resizeEvent(QResizeEvent*);
    void paintEvent(QPaintEvent*);
    double scale_;
    QPixmap original_, scaled_;
};

//! Pure virtual base class for DataImageTab and CorrImageTab, shows a detector image and controls.

class ImageTab : public QcrWidget {
public:
    ImageTab() = delete;
    ImageTab(const QString&);
protected:
    void render();
    virtual QPixmap pixmap() = 0;
    QPixmap makePixmap(const Image&);
    QPixmap makeOverlayPixmap(const class Measurement*);
    QPixmap makeBlankPixmap();
    QImage makeImage(const Image&);
    ImageView imageView_;
    QHBoxLayout box_;
    QVBoxLayout controls_;
    QHBoxLayout box1_;
private:
    QcrIconToggleButton btnScale_;
    QcrIconToggleButton btnOverlay_;
};

//! A tab for a data image and associated controls.

//! To display a 2d detector image of diffraction data,
//! as opposed to the correction data in CorrImageTab.

class DataImageTab : public ImageTab {
public:
    DataImageTab();
    ~DataImageTab();
private:
    const Measurement* measurement();
    QGridLayout boxIdx_;
    QGridLayout boxRanges_;
    QPixmap pixmap() final;
    QcrIconToggleButton btnShowBins_;
    QLabel gammaRangeTotal_{"gammaRangeTotal"};
    QLabel gammaRangeSlice_{"gammaRangeSlice"};
    QLabel thetaRangeTotal_{"thetaRangeTotal"};
    QLabel thetaRangeBin_  {"thetaRangeBin"};
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
