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

#include "plot_diagram.h"
#include "core/def/idiomatic_for.h"
#include "core/session.h"
#include "gui/mainwin.h"
#include "gui/state.h"
#include "qcr/base/debug.h"
#include "qcr/widgets/controls.h"

//  ***********************************************************************************************
//! @class PlotDiagram

PlotDiagram::PlotDiagram()
{
    graph_ = addGraph();
    graphLo_ = addGraph();
    graphUp_ = addGraph();

    graph_->setPen(QPen(Qt::blue));
    graphUp_->setPen(QPen(Qt::red));
    graphLo_->setPen(QPen(Qt::green));
}

void PlotDiagram::refresh()
{
    graph_->clearData();
    graphUp_->clearData();
    graphLo_->clearData();

    const int xi = int(gGui->state->diagramX->getValue());
    const int yi = int(gGui->state->diagramY->getValue());

    std::vector<double> xs, ys, ysLow, ysHig;
    const PeakInfos* peakInfos = gSession->allPeaks.currentPeaks();
    ASSERT(peakInfos);
    peakInfos->get4(xi, yi, xs, ys, ysLow, ysHig);

    if (!xs.size())
        return erase();

    Range rgeX(xs);
    Range rgeY(ys);
    if (rgeX.isEmpty() || rgeY.isEmpty())
        return erase();

    xAxis->setRange(rgeX.min, rgeX.max);
    yAxis->setRange(rgeY.min, rgeY.max);
    xAxis->setVisible(true);
    yAxis->setVisible(true);

    graph_->addData(QVector<double>::fromStdVector(xs), QVector<double>::fromStdVector(ys));
    graphUp_->addData(QVector<double>::fromStdVector(xs),QVector<double>::fromStdVector( ysHig));
    graphLo_->addData(QVector<double>::fromStdVector(xs),QVector<double>::fromStdVector( ysLow));

    replot();
}

void PlotDiagram::erase()
{
    xAxis->setVisible(false);
    yAxis->setVisible(false);
    replot();
}
