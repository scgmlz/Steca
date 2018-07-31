//  ***********************************************************************************************
//
//  Steca: stress and texture calculator
//
//! @file      gui/view/plot_dfgram.h
//! @brief     Defines classes PlotDfgram and Dfgram.
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
//  ***********************************************************************************************

#ifndef PLOT_DFGRAM_H
#define PLOT_DFGRAM_H

#include "core/typ/curve.h"
#include "gui/mainwin.h" // for eFittingTab
#include "gui/view/plot_overlay.h"

//! Listens to mouse events to select subranges of a PlotDfgram.

//! Equips PlotOverlay with domain-specific colors and setter functions.

class PlotDfgramOverlay : public PlotOverlay {
public:
    PlotDfgramOverlay(class PlotDfgram&);

private:
    void addRange(const Range&) final;
    void selectRange(double x) final;
    bool addModeColor(QColor&) const final;
};


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
    void onPeakData();

    QCPGraph *bgGraph_;
    QCPGraph *dgramGraph_;
    QCPGraph *dgramBgFittedGraph_;
    QCPGraph *dgramBgFittedGraph2_;
    QCPGraph *guesses_;
    QCPGraph *fits_;
    std::vector<QCPGraph*> reflGraph_;
    PlotDfgramOverlay* overlay_;

    void calcDfgram();
    void calcBackground();
    void calcPeaks();
    Curve curve0_;
    Curve dgramBgFitted_;
    Curve bg_;
    std::vector<Curve> refls_;
    int currReflIndex_ {0};
};

#endif // PLOT_DFGRAM_H
