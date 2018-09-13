//  ***********************************************************************************************
//
//  Steca: stress and texture calculator
//
//! @file      gui/view/plot_overlay.h
//! @brief     Defines class PlotOverlay
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
//  ***********************************************************************************************

#ifndef PLOT_OVERLAY_H
#define PLOT_OVERLAY_H

#include "core/typ/range.h"
#include "QCustomPlot/qcustomplot.h"

//! Listens to mouse events to select subranges of a plot.

class PlotOverlay : public QWidget /* sic, no QcrWidget here */ {
public:
    PlotOverlay(QCustomPlot&, double _step);
    PlotOverlay(const PlotOverlay&) = delete;

    void setMargins(int left, int right);

protected:
    virtual void addRange(const Range&) = 0;
    virtual void selectRange(double x) = 0;
    virtual const QColor* mousedColor() const = 0;

    QCustomPlot& plot_;

private:
    void enterEvent(QEvent*) final;
    void leaveEvent(QEvent*) final;
    void mousePressEvent(QMouseEvent*) final;
    void mouseReleaseEvent(QMouseEvent*) final;
    void mouseMoveEvent(QMouseEvent*) final;
    void paintEvent(QPaintEvent*) final;
    void paintMousedZone();
    void paintCursor();
    void updateCursorRegion();

    double pix2roundedCoord(double) const;
    double pix2roundedPixel(double) const;

    int marginLeft_, marginRight_;
    int cursorPos_, mouseDownPos_;
    bool hasCursor_, mouseDown_;
    Qt::MouseButton mouseButton_;
    double step_;
};

#endif // PLOT_OVERLAY_H
