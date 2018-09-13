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
//#include "qcr/base/debug.h"
#include "QCustomPlot/qcustomplot.h"

namespace {

std::vector<PolefigPoint> computePoints(const bool flat, const bool withHighlight)
{
    const InfoSequence* allPeaks = gSession->allPeaks.currentInfoSequence();
    if (!allPeaks)
        return {};

    std::vector<PolefigPoint> ret;
    if (flat) {
        for (const PeakInfo& r : allPeaks->peaks())
            ret.push_back({r.alpha(), r.beta(), .2, false});

    } else {
        double rgeMax = 0;
        for (const PeakInfo& r : allPeaks->peaks())
            rgeMax = std::max(rgeMax, r.inten());
        for (const PeakInfo& r : allPeaks->peaks()) {
            bool highlight = false;
            if (withHighlight)
                highlight = false; // TODO find out whether this comes from highlighted cluster
            ret.push_back({r.alpha(), r.beta(), r.inten()/rgeMax, highlight});
        }
    }
    return ret;
}

//! Color map for polefigure: shades of blue.
QColor intenGraph(double inten, bool highlight) {
    if (!qIsFinite(inten))
        return { qRgb(0xff, 0x00, 0x00) };
    int saturation = 0xff - (int)(0xff * inten) / 3;
    if (highlight)
        return { qRgb(0, saturation, saturation) };
    return { qRgb(0, 0, saturation) };
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

void paintPoints(QPainter& painter, const std::vector<PolefigPoint>& points, const double radius)
{
    for (const PolefigPoint& p : points) {
        const QPointF& pp = angles2xy(radius, p.alpha, p.beta);
        QColor color = intenGraph(p.intensity, p.highlight);
        painter.setPen(color);
        painter.setBrush(color);
        if (qIsFinite(p.intensity))
            circle(painter, pp, p.intensity * radius / 60);
        else
            circle(painter, pp, .2 * radius / 60);
    }
}

} //namespace


PlotPolefig::PlotPolefig(const bool alive)
{
    if (alive)
        setRemake([this](){
                points_ = computePoints(flat.val(), true);
                QWidget::update(); // Which then calls paintEvent. Only so we can use QPainter.
            });
    else
        points_ = computePoints(flat.val(), false);
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
    paintPoints(painter, points_, radius);
}
