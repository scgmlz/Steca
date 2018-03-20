// ************************************************************************** //
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
// ************************************************************************** //

#ifndef TABIMAGE_H
#define TABIMAGE_H

#include "gui/mainwin.h"
#include "gui/base/controls.h"
#include <QPainter>

//! Displays a 2d detector image, and possibly some overlay. Used in ImageTab.

class ImageWidget final : public QWidget {
public:
    ImageWidget();
    void setPixmap(const QPixmap&);
    void setScale();
private:
    void resizeEvent(QResizeEvent*);
    void paintEvent(QPaintEvent*);
    qreal scale_;
    QPixmap original_, scaled_;
};

//! A spin box that governs which measurement out of the highlighted group shall be shown.

class IdxMeas : public CSpinBox {
public:
    IdxMeas();
private:
    void fromCore();
};

//! Pure virtual base class for DataImageTab and CorrImageTab, shows a detector image and controls.

class ImageTab : public QWidget {
public:
    ImageTab();
    void render();
protected:
    virtual QPixmap pixmap() = 0;
    QPixmap makePixmap(shp_Image);
    QPixmap makeOverlayPixmap(const class Measurement&);
    QPixmap makeBlankPixmap();
    QImage makeImage(shp_Image);
    ImageWidget imageView_;
    QHBoxLayout box_;
    QVBoxLayout controls_;
    QHBoxLayout box1_;
private:
    XIconButton btnScale_;
    XIconButton btnOverlay_;
};

//! A tab for a data image and associated controls.

//! To display a 2d detector image of diffraction data,
//! as opposed to the correction data in CorrImageTab.

class DataImageTab : public ImageTab {
public:
    DataImageTab();
    ~DataImageTab();
private:
    QGridLayout boxIdx_;
    QGridLayout boxRanges_;
    QPixmap pixmap() final;
    IdxMeas idxMeas_;
    CSpinBox idxSlice_{"idxSlice", 4, false, 1, INT_MAX, "Number of γ slice to be shown" };
    CSpinBox idxTheta_ {"idxTheta", 4, false, 1, INT_MAX, "Number of 2θ bin to be shown" };
    QLabel gammaRangeTotal_;
    QLabel gammaRangeSlice_;
    QLabel thetaRangeTotal_;
    QLabel thetaRangeBin_;
    XIconButton btnShowBins_;
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

#endif // TABIMAGE_H
