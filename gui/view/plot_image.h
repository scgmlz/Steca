//  ***********************************************************************************************
//
//  Steca: stress and texture calculator
//
//! @file      gui/panels/tab_image.cpp
//! @brief     Defines class ImageView
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
//  ***********************************************************************************************

#include "qcr/widgets/views.h"

//! Displays a 2d detector image, and possibly some overlay. Used in ImageTab.

class ImageView final : public QcrWidget {
public:
    ImageView();
    void setPixmap(const QPixmap&);
    void setScale();
private:
    void resizeEvent(QResizeEvent*);
    void paintEvent(QPaintEvent*);
    double scale_ {0};
    QPixmap original_, scaled_;
};
