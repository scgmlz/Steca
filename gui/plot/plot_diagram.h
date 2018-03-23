// ************************************************************************** //
//
//  Steca: stress and texture calculator
//
//! @file      gui/plot/plot_diagram.h
//! @brief     Defines class PlotDiagram
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
// ************************************************************************** //

#ifndef PLOT_DIAGRAM_H
#define PLOT_DIAGRAM_H

#include "core/data/peak_info.h"
#include "QCustomPlot/qcustomplot.h"

//! Tab in DiagramsFrame, to display a plot of something against something.

class PlotDiagram : public QCustomPlot {
public:
    PlotDiagram();
    void set(PeakInfos);
    void plot(const QVector<qreal>& xs, const QVector<qreal>& ys,
              const QVector<qreal>& ysLo, const QVector<qreal>& ysUp);
private:
    QCPGraph *graph_, *graphLo_, *graphUp_;
};

#endif // PLOT_DIAGRAM_H
