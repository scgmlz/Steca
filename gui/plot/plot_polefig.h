// ************************************************************************** //
//
//  Steca: stress and texture calculator
//
//! @file      gui/plot/plot_polefig.h
//! @brief     Defines class PlotPolefig
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
// ************************************************************************** //

#ifndef PLOT_POLEFIG_H
#define PLOT_POLEFIG_H

#include "core/data/peak_info.h"
#include "gui/base/controls.h"
#include "QCustomPlot/qcustomplot.h"

//! Tab in PoleFiguresFrame, to display the pole figure.

class PlotPolefig : public QWidget {
public:
    PlotPolefig();
    void set(const PeakInfos&);

private:
    QGridLayout* grid_;
    void update();

    PeakInfos rs_;
    void paintEvent(QPaintEvent*);

    QPointF p(deg alpha, deg beta) const;
    deg alpha(const QPointF&) const;
    deg beta(const QPointF&) const;

    void circle(QPointF c, qreal r);

    void paintGrid();
    void paintPoints();

    // valid during paintEvent
    QPainter* p_;
    QPointF c_;
    qreal r_;

    bool flat_;
    qreal alphaMax_, avgAlphaMax_;

    CCheckBox cbFlat_;
};

#endif // PLOT_POLEFIG_H
