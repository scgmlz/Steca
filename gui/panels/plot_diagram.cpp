// ************************************************************************** //
//
//  Steca: stress and texture calculator
//
//! @file      gui/panels/plot_diagram.cpp
//! @brief     Implements class PlotDiagram
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
// ************************************************************************** //

#include "plot_diagram.h"
#include "core/def/idiomatic_for.h"

// ************************************************************************** //
//  local class PlotDiagram
// ************************************************************************** //

//! Tab in DiagramsFrame, to display a plot of something against something.

PlotDiagram::PlotDiagram()
{
    graph_ = addGraph();
    graphLo_ = addGraph();
    graphUp_ = addGraph();
}

void PlotDiagram::plot(
    const vec<qreal>& xs, const vec<qreal>& ys, const vec<qreal>& ysLo, const vec<qreal>& ysUp)
{
    ASSERT(xs.count() == ys.count());

    int count = xs.count();

    graph_->clearData();
    graphUp_->clearData();
    graphLo_->clearData();

    Range rgeX, rgeY;

    for_i (count) {
        rgeX.extendBy(xs.at(i));
        rgeY.extendBy(ys.at(i));
    }

    if (!count || rgeX.isEmpty() || rgeY.isEmpty()) {
        xAxis->setVisible(false);
        yAxis->setVisible(false);
        replot();
        return;
    }

    xAxis->setRange(rgeX.min, rgeX.max);
    yAxis->setRange(rgeY.min, rgeY.max);
    xAxis->setVisible(true);
    yAxis->setVisible(true);

    graph_->setPen(QPen(Qt::blue));
    graph_->addData(xs.sup(), ys.sup());

    graphUp_->setPen(QPen(Qt::red));
    graphUp_->addData(xs.sup(), ysUp.sup());

    graphLo_->setPen(QPen(Qt::green));
    graphLo_->addData(xs.sup(), ysLo.sup());

    replot();
}
