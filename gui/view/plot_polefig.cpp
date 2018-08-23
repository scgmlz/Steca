//  ***********************************************************************************************
//
//  Steca: stress and texture calculator
//
//! @file      gui/view/plot_polefig.cpp
//! @brief     Implements class PlotPolefig
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
//  ***********************************************************************************************

#include "gui/view/plot_polefig.h"
#include "core/session.h"
#include "core/aux/angles.h"
#include "qcr/base/debug.h" // ASSERT
#include "QCustomPlot/qcustomplot.h"

namespace {

//! Color map for polefigure: shades of blue.
QColor intenGraph(double inten, double maxInten) {
    if (!qIsFinite(inten) || qIsNaN(maxInten) || maxInten <= 0)
        return { qRgb(0x00, 0x00, 0x00) };
    inten /= maxInten;
    return { qRgb(0, 0, int(0xff * (1 - inten / 3))) };
}

//! Point in floating-point precision
QPointF angles2xy(double radius, deg alpha, deg beta)
{
    double r = radius * alpha / 90;
    rad betaRad = beta.toRad();
    return QPointF(r * cos(betaRad), -r * sin(betaRad));
}

void circle(QPainter& painter, QPointF c, double r)
{
    painter.drawEllipse(c, r, r);
}

void paintGrid(QPainter& painter, const double radius)
{
    QPen penMajor(Qt::gray), penMinor(Qt::lightGray);
    QPointF centre(0, 0);

    for (int alpha = 10; alpha <= 90; alpha += 10) {
        double r = radius * alpha / 90;
        painter.setPen(!(alpha % 30) ? penMajor : penMinor);
        circle(painter, centre, r);
    }

    for (int beta = 0; beta < 360; beta += 10) {
        painter.setPen(!(beta % 30) ? penMajor : penMinor);
        painter.drawLine(angles2xy(radius, 10, beta), angles2xy(radius, 90, beta));
    }

    QPen penMark(Qt::darkGreen);
    painter.setPen(penMark);
    double avgAlphaMax = gSession->params.interpolParams.avgAlphaMax.val();
    circle(painter, centre, radius * avgAlphaMax / 90);
}


void paintPoints(
    QPainter& painter, const InfoSequence& peakInfos, const bool flat, const double radius)
{
    double rgeMax = 0;
    for (const PeakInfo& r : peakInfos.peaks())
        rgeMax = std::max(rgeMax, r.inten());

    for (const PeakInfo& r : peakInfos.peaks()) {
        double inten = r.inten();
        if (!qIsFinite(inten)) // NaN's may occur in interpolated (but not in direct) peakInfos
            continue;
        const QPointF& pp = angles2xy(radius, r.alpha(), r.beta());
        if (flat) {
            QColor color(Qt::blue);
            painter.setPen(color);
            painter.setBrush(color);
            circle(painter, pp, .5);
        } else {
            inten /= rgeMax;
            QColor color = intenGraph(inten, 1);
            painter.setPen(color);
            painter.setBrush(color);
            circle(painter, pp, inten * radius / 60); // TODO scale to max inten
        }
    }
}

} //namespace


PlotPolefig::PlotPolefig()
{
    setRemake([this](){
            peakInfos_ = gSession->allPeaks.currentInfoSequence();
            QWidget::update(); // Which then calls paintEvent. Only so we can use QPainter.
        });
}

//! Plots the figure, using cached data points (which are computed by remake()).
void PlotPolefig::paintEvent(QPaintEvent*)
{
    int w = size().width();
    int h = size().height();
    double radius = qMin(w, h) / 2;

    QPainter painter{this};
    painter.setRenderHint(QPainter::Antialiasing);
    painter.translate(w / 2, h / 2);

    paintGrid(painter, radius);
    if (peakInfos_) {
        ASSERT(peakInfos_);
        paintPoints(painter, *peakInfos_, flat.val(), radius);
    }
}
