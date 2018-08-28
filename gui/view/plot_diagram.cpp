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
#include "qcr/widgets/controls.h"
//#include "qcr/base/debug.h"

//  ***********************************************************************************************
//! @class PlotDiagram

PlotDiagram::PlotDiagram()
{
    graph_   = addGraph();

    graph_->setScatterStyle(QCPScatterStyle::ssCircle);
    graph_->setPen(QPen(Qt::blue));
    //graph_->setErrorBarSize(1);
    graph_->setErrorPen(QPen(Qt::black));
}

PlotDiagram::PlotDiagram(int w, int h)
    : PlotDiagram()
{
    setMinimumSize(w, h);
}

void PlotDiagram::refresh()
{
    if (!gSession->activeClusters.size() || !gSession->peaks.size())
        return;

    graph_  ->clearData();

    const int idxX = int(gSession->params.diagramX.val());
    const int idxY = int(gSession->params.diagramY.val());

    std::vector<double> xs, ys, ysSigma;
    gSession->allPeaks.currentInfoSequence()->getValuesAndSigma(idxX, idxY, xs, ys, ysSigma);
    //std::vector<double> xs, ys, ysLow, ysHig;
    //gSession->allPeaks.currentInfoSequence()->get4(idxX, idxY, xs, ys, ysLow, ysHig);


    if (!xs.size())
        return erase();

    Range rgeX(xs);
    Range rgeY(ys);
    //Range rgeY = Range(ysLow).intersect(Range(ysHig));
    if (rgeX.isEmpty() || rgeY.isEmpty())
        return erase();

    xAxis->setRange(rgeX.min, rgeX.max);
    yAxis->setRange(rgeY.min, rgeY.max);
    xAxis->setVisible(true);
    yAxis->setVisible(true);
    graph_->setErrorType(QCPGraph::ErrorType::etValue);
    graph_->setDataValueError(QVector<double>::fromStdVector(xs), QVector<double>::fromStdVector(ys), QVector<double>::fromStdVector(ysSigma));
    replot();
}

void PlotDiagram::erase()
{
    xAxis->setVisible(false);
    yAxis->setVisible(false);
    replot();
}
