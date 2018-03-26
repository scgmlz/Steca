// ************************************************************************** //
//
//  Steca: stress and texture calculator
//
//! @file      gui/plot/plot_diagram.cpp
//! @brief     Implements class PlotDiagram
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
// ************************************************************************** //

#include "plot_diagram.h"
#include "core/def/debug.h"
#include "core/def/idiomatic_for.h"
#include "core/session.h"
#include "gui/mainwin.h"
#include "gui/state.h"
#include "gui/base/controls.h"

namespace {

// sorts xs and ys the same way, by (x,y)
static void sortColumns(QVector<double>& xs, QVector<double>& ys, QVector<int>& is)
{
    ASSERT(xs.count() == ys.count());

    int count = xs.count();

    is.resize(count);
    for_i (count)
        is[i] = i;

    std::sort(is.begin(), is.end(), [&xs, &ys](int i1, int i2) {
        double x1 = xs.at(i1), x2 = xs.at(i2);
        if (x1 < x2)
            return true;
        if (x1 > x2)
            return false;
        return ys.at(i1) < ys.at(i2);
    });

    QVector<double> r(count);

    for_i (count)
        r[i] = xs.at(is.at(i));
    xs = r;

    for_i (count)
        r[i] = ys.at(is.at(i));
    ys = r;
}


} // namespace


// ************************************************************************** //
//  class PlotDiagram
// ************************************************************************** //

PlotDiagram::PlotDiagram()
{
    graph_ = addGraph();
    graphLo_ = addGraph();
    graphUp_ = addGraph();
}

void PlotDiagram::refresh()
{
    QVector<double> xs, ys, ysLow, ysHig;

    const PeakInfos& peakInfos = gSession->peakInfos();
    int count = peakInfos.count();

    xs.resize(count);
    ys.resize(count);

    int xi = int(gGui->state->diagramX->currentIndex());
    int yi = int(gGui->state->diagramY->currentIndex());

    for_i (count) {
        const row_t row = peakInfos.at(i).data();
        xs[i] = row.at(xi).toDouble();
        ys[i] = row.at(yi).toDouble();
    }

    QVector<int> is;
    sortColumns(xs, ys, is);

    using eReflAttr = PeakInfo::eReflAttr;
    eReflAttr ye = (eReflAttr) yi;

    int iRefl = gSession->peaks().selectedIndex();

    if (!gSession->peaks().at(iRefl).isRaw()
        && (ye==eReflAttr::INTEN || ye==eReflAttr::TTH || ye==eReflAttr::FWHM)) {

        ysLow.resize(count);
        ysHig.resize(count);

        for_i (count) {
            const row_t row = peakInfos.at(is.at(i)).data(); // access error over sorted index vec
            double sigma = row.at(yi+1).toDouble(); // SIGMA_X has tag position of X plus 1
            double y = ys.at(i);
            ysLow[i] = y - sigma;
            ysHig[i] = y + sigma;
        }
    }

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
        //qDebug() << "plot diagram disabled: count=" << count
        //         << ", rgeX=" << rgeX.to_s()
        //         << ", rgeY=" << rgeY.to_s();
        replot();
        return;
    }

    xAxis->setRange(rgeX.min, rgeX.max);
    yAxis->setRange(rgeY.min, rgeY.max);
    xAxis->setVisible(true);
    yAxis->setVisible(true);

    graph_->setPen(QPen(Qt::blue));
    graph_->addData(xs, ys);

    graphUp_->setPen(QPen(Qt::red));
    graphUp_->addData(xs, ysHig);

    graphLo_->setPen(QPen(Qt::green));
    graphLo_->addData(xs, ysLow);

    replot();
}
