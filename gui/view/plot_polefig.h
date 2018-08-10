//  ***********************************************************************************************
//
//  Steca: stress and texture calculator
//
//! @file      gui/view/plot_polefig.h
//! @brief     Defines class PlotPolefig
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
//  ***********************************************************************************************

#ifndef PLOT_POLEFIG_H
#define PLOT_POLEFIG_H

#include "QCustomPlot/qcustomplot.h"
#include "core/calc/peak_info.h"
#include "qcr/widgets/controls.h"
#include "qcr/widgets/views.h"

//! Tab in PoleFiguresFrame, to display the pole figure.

class PlotPolefig : public QcrWidget {
public:
    PlotPolefig();

    QcrCell<bool> flat {false};

private:
    void paintEvent(QPaintEvent*);

    QPointF p(deg alpha, deg beta) const;
    deg alpha(const QPointF&) const;
    deg beta(const QPointF&) const;

    void circle(QPointF c, double r);

    void paintGrid();
    void paintPoints();

    const PeakInfos* peakInfos_ {nullptr};

    // valid while painting
    QPainter* p_ {nullptr};
    QPointF c_;
    double r_;

    const double alphaMax_ {90};
};

#endif // PLOT_POLEFIG_H
