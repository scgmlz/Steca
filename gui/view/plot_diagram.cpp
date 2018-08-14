//  ***********************************************************************************************
//
//  Steca: stress and texture calculator
//
//! @file      gui/view/plot_diagram.cpp
//! @brief     Implements class PlotDiagram
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
//  ***********************************************************************************************

#include "gui/view/plot_diagram.h"
#include "core/session.h"
#include "gui/mainwin.h"
#include "gui/state.h"
#include "qcr/base/debug.h"
#include "qcr/widgets/controls.h"
#include "QCustomPlot/qcustomplot.h"

//  ***********************************************************************************************
//! @class PlotDiagram

PlotDiagram::PlotDiagram()
{
    plotter_ = new QCustomPlot(this);

    graph_   = plotter_->addGraph();
    graphLo_ = plotter_->addGraph();
    graphUp_ = plotter_->addGraph();

    graph_  ->setPen(QPen(Qt::blue));
    graphUp_->setPen(QPen(Qt::red));
    graphLo_->setPen(QPen(Qt::green));
}

PlotDiagram::PlotDiagram(int w, int h)
    : PlotDiagram()
{
    plotter_->setMinimumSize(w, h);
}

void PlotDiagram::refresh()
{
    if (!gSession->activeClusters.size() || !gSession->peaks.count())
        return;

    graph_  ->clearData();
    graphUp_->clearData();
    graphLo_->clearData();

    const int idxX = int(gGui->state->diagramX->getValue());
    const int idxY = int(gGui->state->diagramY->getValue());

    std::vector<double> xs, ys, ysLow, ysHig;
    gSession->allPeaks.curentPeakInfos()->get4(idxX, idxY, xs, ys, ysLow, ysHig);

    if (!xs.size())
        return erase();

    Range rgeX(xs);
    Range rgeY(ys);
    qDebug() << "PLODI xrange" << rgeX.to_s();
    if (rgeX.isEmpty() || rgeY.isEmpty())
        return erase();

    plotter_->xAxis->setRange(rgeX.min, rgeX.max);
    plotter_->yAxis->setRange(rgeY.min, rgeY.max);
    plotter_->xAxis->setVisible(true);
    plotter_->yAxis->setVisible(true);

    graph_  ->addData(QVector<double>::fromStdVector(xs), QVector<double>::fromStdVector(ys));
    graphUp_->addData(QVector<double>::fromStdVector(xs), QVector<double>::fromStdVector(ysHig));
    graphLo_->addData(QVector<double>::fromStdVector(xs), QVector<double>::fromStdVector(ysLow));

    plotter_->replot();
}

void PlotDiagram::erase()
{
    plotter_->xAxis->setVisible(false);
    plotter_->yAxis->setVisible(false);
    plotter_->replot();
}
