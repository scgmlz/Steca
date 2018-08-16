//  ***********************************************************************************************
//
//  Steca: stress and texture calculator
//
//! @file      gui/view/plot_dfgram.h
//! @brief     Defines class PlotDfgram.
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
//  ***********************************************************************************************

#ifndef PLOT_DFGRAM_H
#define PLOT_DFGRAM_H

#include "core/typ/range.h"
#include "QCustomPlot/qcustomplot.h"

//! A plot frame that displays diffractogram, background and peak fits, and fit ranges.

class PlotDfgram : public QCustomPlot {
public:
    PlotDfgram();
    PlotDfgram(const PlotDfgram&) = delete;

    void plotEmpty();
    void renderAll();
    void clearReflLayer();
    void enterZoom(bool);

private:
    void addBgItem(const Range&, const QColor&);
    void resizeEvent(QResizeEvent*);

    QCPGraph *bgGraph_;
    QCPGraph *dgramGraph_;
    QCPGraph *dgramBgFittedGraph_;
    QCPGraph *dgramBgFittedGraph2_;
    QCPGraph *guesses_;
    QCPGraph *fits_;
    std::vector<QCPGraph*> reflGraph_;
    class PlotDfgramOverlay* overlay_;
};

#endif // PLOT_DFGRAM_H
