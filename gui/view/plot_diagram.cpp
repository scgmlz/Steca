
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
#include <algorithm>
//#include "qcr/base/debug.h"

namespace {

//! Sets axis according to data range, leaving some white space below and above.

void setRange(QCPAxis* axis, const std::vector<double>& datArg)
{
    std::vector<double> dat = datArg;
    std::sort(dat.begin(), dat.end());
    auto last = std::unique(dat.begin(), dat.end());
    dat.erase(last, dat.end());
    const int N = dat.size();
    double dmin = *std::min_element(dat.begin(), dat.end());
    double dmax = *std::max_element(dat.begin(), dat.end());
    if      (N<1)
        qFatal("plot_diagram{} setRange: impossible case");
    else if (N==1 || dmin==dmax) {
        ASSERT(dmin==dmax);
        double v = dmin;
        if      (v==0)
            axis->setRange(-1, 1);
        else if (v<0)
            axis->setRange(-2*v,0);
        else
            axis->setRange(0, 2*v);
    } else {
        ASSERT(dmin<dmax);
        double width = dmax - dmin;
        // white space on either side
        double border = (N==2 ? .2 : N==3 ? .15 : .12) * width;
        // axis will range from vmin to vmax
        double amin = dmin-border;
        double amax = dmax+border;
        axis->setRange(amin, amax);
    }
}

} // namespace


PlotDiagram::PlotDiagram()
{
    graph_ = addGraph();

    // copy, modify, write back the symbol plot style
    QCPScatterStyle ss = graph_->scatterStyle();
    ss.setBrush(QBrush(Qt::BrushStyle::NoBrush));
    ss.setShape(QCPScatterStyle::ssCircle);
    ss.setSize(5);
    graph_->setScatterStyle(ss);

    graph_->setLineStyle(QCPGraph::LineStyle::lsNone);
    graph_->setPen(QPen{Qt::blue});
    graph_->setErrorPen(QPen{Qt::black});
}

PlotDiagram::PlotDiagram(int w, int h)
    : PlotDiagram{}
{
    setMinimumSize(w, h);
}

void PlotDiagram::refresh()
{
    if (!gSession->activeClusters.size() || !gSession->peaksSettings.size())
        return;

    graph_->clearData();

    const int idxX = gSession->params.diagramX.val();
    const int idxY = gSession->params.diagramY.val();

    std::vector<double> xs, ys, ysSigma;
    gSession->peaksOutcome.currentInfoSequence()->getValuesAndSigma(idxX, idxY, xs, ys, ysSigma);

    std::vector<double> xsSafe, ysSafe, ysSigmaSafe;
    if (ysSigma.size() > 0) {// has valueError
        for (size_t i = 0; i < xs.size(); ++i) {
            if (   qIsNaN(xs.at(i)) || qIsInf(xs.at(i))
                || qIsNaN(ys.at(i)) || qIsInf(ys.at(i)))
                continue;
            xsSafe.push_back(xs.at(i));
            ysSafe.push_back(ys.at(i));
            ysSigmaSafe.push_back(ysSigma.at(i));
        }
        graph_->setErrorType(QCPGraph::ErrorType::etValue);
        graph_->setDataValueError(QVector<double>::fromStdVector(xsSafe),
                                  QVector<double>::fromStdVector(ysSafe),
                                  QVector<double>::fromStdVector(ysSigmaSafe));
    } else {
        for (size_t i = 0; i < xs.size(); ++i) {
            if (   qIsNaN(xs.at(i)) || qIsInf(xs.at(i))
                || qIsNaN(ys.at(i)) || qIsInf(ys.at(i)))
                continue;
            xsSafe.push_back(xs.at(i));
            ysSafe.push_back(ys.at(i));
        }
        graph_->setErrorType(QCPGraph::ErrorType::etNone);
        graph_->setData(QVector<double>::fromStdVector(xsSafe),
                        QVector<double>::fromStdVector(ysSafe));
    }

    if (!xsSafe.size())
        return erase();

    setRange(xAxis, xsSafe);
    setRange(yAxis, ysSafe);
    xAxis->setVisible(true);
    yAxis->setVisible(true);

    replot();
}

void PlotDiagram::erase()
{
    xAxis->setVisible(false);
    yAxis->setVisible(false);
    replot();
}
