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

#include "qcr/engine/cell.h"
#include "qcr/widgets/views.h"
#include <memory>

struct PolefigPoint {
    double alpha;
    double beta;
    double intensity;
    bool   highlight;
};

//! Tab in PoleFiguresFrame, to display the pole figure.

class PlotPolefig : public QcrWidget {
public:
    PlotPolefig(bool alive);

    QcrCell<bool> flat {false}; //!< Show only grid points, and no intensity representation

private:
    void paintEvent(QPaintEvent*);

    std::vector<PolefigPoint> points_;
};

#endif // PLOT_POLEFIG_H
