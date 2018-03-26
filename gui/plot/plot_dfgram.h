// ************************************************************************** //
//
//  Steca: stress and texture calculator
//
//! @file      gui/plot/plot_dfgram.h
//! @brief     Defines classes DiffractogramPlot and Diffractogram.
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
// ************************************************************************** //

#ifndef PLOT_DFGRAM_H
#define PLOT_DFGRAM_H

#include "gui/mainwin.h" // for eFittingTab
#include "gui/base/plot_overlay.h"

//! Listens to mouse events to select subranges of a DiffractogramPlot.

//! Equips PlotOverlay with domain-specific colors and setter functions.

class DiffractogramPlotOverlay : public PlotOverlay {
public:
    DiffractogramPlotOverlay(class DiffractogramPlot&);

private:
    void addRange(const Range&) final;
    void subtractRange(const Range&) final;
    bool addModeColor(QColor&) const final;
    bool subtractModeColor(QColor&) const final;
};


//! A plot frame that displays diffractogram, background and peak fits, and fit ranges.

class DiffractogramPlot : public QCustomPlot {
public:
    DiffractogramPlot(class Diffractogram&);

    void plot(const Curve&, const Curve&, const Curve&, const curve_vec&, int);
    void plotEmpty();
    void renderAll();
    void clearReflLayer();
    void enterZoom(bool);

private:
    void addBgItem(const Range&, const QColor&);
    void resizeEvent(QResizeEvent*);
    void onPeakData();

    Diffractogram& diffractogram_;

    bool showBgFit_;
    QCPGraph *bgGraph_, *dgramGraph_, *dgramBgFittedGraph_, *dgramBgFittedGraph2_, *guesses_,
        *fits_;
    QVector<QCPGraph*> reflGraph_;
    DiffractogramPlotOverlay* overlay_;

    void calcDgram();
    void calcBackground();
    void calcPeaks();
    Curve dgram_, dgramBgFitted_, bg_;
    curve_vec refls_;
    int currReflIndex_ {0};
};

#endif // PLOT_DFGRAM_H
