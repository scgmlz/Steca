//  ***********************************************************************************************
//
//  Steca: stress and texture calculator
//
//! @file      gui/view/plot_overlay.cpp
//! @brief     Implements class PlotOverlay
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
//  ***********************************************************************************************

#include "gui/view/plot_overlay.h"
//#include "qcr/base/debug.h"

//  ***********************************************************************************************
//! @class PlotOverlay

PlotOverlay::PlotOverlay(QCustomPlot& _plot, double _step)
    : QWidget(&_plot)
    , plot_(_plot)
    , cursorPos_(0)
    , mouseDownPos_(0)
    , hasCursor_(false)
    , mouseDown_(false)
    , step_(_step)
{
    setMouseTracking(true);
    setMargins(0, 0);
}

void PlotOverlay::setMargins(int left, int right)
{
    marginLeft_ = left;
    marginRight_ = right;
}

void PlotOverlay::enterEvent(QEvent*)
{
    hasCursor_ = true;
    updateCursorRegion();
}

void PlotOverlay::leaveEvent(QEvent*)
{
    hasCursor_ = false;
    updateCursorRegion();
}

void PlotOverlay::mousePressEvent(QMouseEvent* e)
{
    mouseDownPos_ = cursorPos_;
    mouseDown_ = true;
    mouseButton_ = e->button();
    update();
}

void PlotOverlay::mouseReleaseEvent(QMouseEvent* e)
{
    mouseDown_ = false;
    update();
    double xold = pix2roundedCoord(mouseDownPos_);
    double xnew = pix2roundedCoord(cursorPos_);
    if (xnew == xold) { // clicked not moved
        selectRange(xnew);
        return;
    }
    if      (e->button()==Qt::LeftButton)
        addRange(Range::safeFrom(xold, xnew));
}

void PlotOverlay::mouseMoveEvent(QMouseEvent* e)
{
    updateCursorRegion();
    cursorPos_ = qBound(marginLeft_, e->x(), width() - marginRight_);
    updateCursorRegion();
    if (mouseDown_)
        update();
}

void PlotOverlay::paintEvent(QPaintEvent*)
{
    if (mouseDown_)
        paintMousedZone();
    if (hasCursor_)
        paintCursor();
}

void PlotOverlay::paintMousedZone()
{
    if (mouseButton_!=Qt::LeftButton)
        return;
    QRect g = geometry();
    g.setLeft (pix2roundedPixel(qMin(mouseDownPos_, cursorPos_)));
    g.setRight(pix2roundedPixel(qMax(mouseDownPos_, cursorPos_)));
    if (const QColor* color = mousedColor())
        QPainter(this).fillRect(g, *color);
}

void PlotOverlay::paintCursor()
{
    QRect g = geometry();
    QLineF cursor(cursorPos_, g.top(), cursorPos_, g.bottom());
    QPainter painter(this);
    painter.setPen(Qt::red);
    painter.drawLine(cursor);
}

void PlotOverlay::updateCursorRegion()
{
    const QRect& g = geometry();
    // updating 2 pixels seems to work both on Linux & Mac
    update(cursorPos_ - 1, g.top(), 2, g.height());
}

double PlotOverlay::pix2roundedCoord(double pix) const
{
    return step_ * std::round(plot_.xAxis->pixelToCoord(pix)/step_);
}

double PlotOverlay::pix2roundedPixel(double pix) const
{
    return plot_.xAxis->coordToPixel(pix2roundedCoord(pix));
}
