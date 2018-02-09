// ************************************************************************** //
//
//  Steca: stress and texture calculator
//
//! @file      gui/panels/panel_diffractogram.h
//! @brief     Defines classes DiffractogramPlot and Diffractogram.
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
// ************************************************************************** //

#ifndef PANEL_DIFFRACTOGRAM_H
#define PANEL_DIFFRACTOGRAM_H

#include "gui/mainwin.h" // for eFittingTab
#include "QCustomPlot/qcustomplot.h"

class DiffractogramPlot;

//! Listens to mouse events to select subranges of the diffractogram plot.

class DiffractogramPlotOverlay : public QWidget {
public:
    DiffractogramPlotOverlay(DiffractogramPlot&);

    void setMargins(int left, int right);

private:
    void enterEvent(QEvent*) final;
    void leaveEvent(QEvent*) final;
    void mousePressEvent(QMouseEvent*) final;
    void mouseReleaseEvent(QMouseEvent*) final;
    void mouseMoveEvent(QMouseEvent*) final;
    void paintEvent(QPaintEvent*) final;
    void paintMousedZone();
    void paintCursor();

    void updateCursorRegion();

    DiffractogramPlot& plot_;

    int marginLeft_, marginRight_;
    int cursorPos_, mouseDownPos_;
    bool hasCursor_, mouseDown_;
    Qt::MouseButton mouseButton_;
};


//! A plot frame that displays diffractogram, background and peak fits, and fit ranges.

class DiffractogramPlot : public QCustomPlot {
public:
    enum class eTool {
        NONE,
        BACKGROUND,
        PEAK_REGION,
    };

    DiffractogramPlot(class Diffractogram&);

    void setTool(eTool);
    Range fromPixels(int, int);
    void plot(Curve const&, Curve const&, Curve const&, curve_vec const&, int);
    void plotEmpty();
    void renderAll();
    void clearReflLayer();
    void enterZoom(bool);

    eTool getTool() const { return tool_; }

private:
    void addBgItem(const Range&);
    void resizeEvent(QResizeEvent*);
    void onPeakData();

    Diffractogram& diffractogram_;

    const QColor BgColor_{0x98, 0xfb, 0x98, 0x50};
    const QColor reflRgeColor_{0x87, 0xce, 0xfa, 0x50};

    eTool tool_;
    bool showBgFit_;
    QCPGraph *bgGraph_, *dgramGraph_, *dgramBgFittedGraph_, *dgramBgFittedGraph2_, *guesses_,
        *fits_;
    vec<QCPGraph*> reflGraph_;
    DiffractogramPlotOverlay* overlay_;

    void calcDgram();
    void calcBackground();
    void calcPeaks();
    Curve dgram_, dgramBgFitted_, bg_;
    curve_vec refls_;
    int currReflIndex_ {0};
};

#endif // PANEL_DIFFRACTOGRAM_H
