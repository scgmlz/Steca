// ************************************************************************** //
//
//  Steca: stress and texture calculator
//
//! @file      gui/panels/plot_diagram.h
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

#include "core/calc/peak_info.h"
#include "core/typ/vec.h"
#include "QCustomPlot/qcustomplot.h"

class PlotDiagram : public QCustomPlot {
public:
    PlotDiagram();
    void set(PeakInfos);
    void plot(
        const vec<qreal>& xs, const vec<qreal>& ys, const vec<qreal>& ysLo, const vec<qreal>& ysUp);
private:
    QCPGraph *graph_, *graphLo_, *graphUp_;
};

#endif // PLOT_DIAGRAM_H
