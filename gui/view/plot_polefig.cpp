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
#include "qcr/base/debug.h"

namespace {

const double alphaMAX {90};

//! Color map for polefigure: shades of blue.
QColor intenGraph(double inten, double maxInten) {
    if (!qIsFinite(inten) || qIsNaN(maxInten) || maxInten <= 0)
        return { qRgb(0x00, 0x00, 0x00) };

    inten /= maxInten;

    return { qRgb(0, 0, int(0xff * (1 - inten / 3))) };
}

} //namespace


PlotPolefig::PlotPolefig()
{
    setRemake ([this](){
            peakInfos_ = gSession->allPeaks.curentPeakInfos();
            QWidget::update(); // Which then calls paintEvent. Only so we can use QPainter.
        });
}

//! Plots the figure, using cached data points (which are computed by remake()).
void PlotPolefig::paintEvent(QPaintEvent*)
{
    qDebug() << "PlotPolefig::refresh()1";

    painter_.reset(new QPainter(this));
    painter_->setRenderHint(QPainter::Antialiasing);
    int w = size().width(), h = size().height();
    painter_->translate(w / 2, h / 2);

    radius_ = qMin(w, h) / 2;

    paintGrid();

    if (peakInfos_)
        paintPoints();
    painter_.reset();
    qDebug() << "PlotPolefig::refresh()9";
}

void PlotPolefig::paintGrid()
{
    QPen penMajor(Qt::gray), penMinor(Qt::lightGray);
    QPointF centre(0, 0);

    for (int alpha = 10; alpha <= 90; alpha += 10) {
        double r = radius_ / alphaMAX * alpha;
        painter_->setPen(!(alpha % 30) ? penMajor : penMinor);
        circle(centre, r);
    }

    for (int beta = 0; beta < 360; beta += 10) {
        painter_->setPen(!(beta % 30) ? penMajor : penMinor);
        painter_->drawLine(angles2xy(10, beta), angles2xy(90, beta));
    }

    QPen penMark(Qt::darkGreen);
    painter_->setPen(penMark);
    double avgAlphaMax = gSession->params.interpolParams.avgAlphaMax.val();
    circle(centre, radius_ * avgAlphaMax / alphaMAX);
}

void PlotPolefig::paintPoints()
{
    double rgeMax = 0;
    ASSERT(peakInfos_);
    for (const PeakInfo& r : peakInfos_->peaks())
        rgeMax = std::max(rgeMax, r.inten());

    for (const PeakInfo& r : peakInfos_->peaks()) {
        double inten = r.inten();
        if (!qIsFinite(inten)) // NaN's may occur in interpolated (but not in direct) peakInfos
            continue;
        const QPointF& pp = angles2xy(r.alpha(), r.beta());
        if (flat.val()) {
            QColor color(Qt::blue);
            painter_->setPen(color);
            painter_->setBrush(color);
            circle(pp, .5);
        } else {
            inten /= rgeMax;
            QColor color = intenGraph(inten, 1);
            painter_->setPen(color);
            painter_->setBrush(color);
            circle(pp, inten * radius_ / 60); // TODO scale to max inten
        }
    }
}

//! Point in floating-point precision
QPointF PlotPolefig::angles2xy(deg alpha, deg beta) const
{
    double r = radius_ * alpha / alphaMAX;
    rad betaRad = beta.toRad();
    return QPointF(r * cos(betaRad), -r * sin(betaRad));
}

void PlotPolefig::circle(QPointF c, double r)
{
    painter_->drawEllipse(c, r, r);
}
