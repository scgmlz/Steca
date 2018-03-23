// ************************************************************************** //
//
//  Steca: stress and texture calculator
//
//! @file      gui/panels/panel_diffractogram.cpp
//! @brief     Implements classes DiffractogramPlot and Diffractogram.
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
// ************************************************************************** //

#include "panel_diffractogram.h"
#include "core/session.h"
#include "gui/actions/toggles.h"
#include "gui/panels/subframe_setup.h"

// ************************************************************************** //
//  class DiffractogramPlotOverlay
// ************************************************************************** //

DiffractogramPlotOverlay::DiffractogramPlotOverlay(DiffractogramPlot& parent)
    : PlotOverlay(parent)
{
}

void DiffractogramPlotOverlay::addRange(const Range& range)
{
    if        (gGui->setup()->editingBaseline()) {
        gSession->baseline().addRange(range);
    } else if (gGui->setup()->editingPeaks()) {
        if (Peak* peak = gSession->peaks().selectedPeak()) {
            peak->setRange(range);
            gConsole->log("peakRangeMin set "+QString::number(range.min));
            gConsole->log("peakRangeMax set "+QString::number(range.max));
        }
    }
}

void DiffractogramPlotOverlay::subtractRange(const Range& range)
{
    if        (gGui->setup()->editingBaseline()) {
        gSession->baseline().removeRange(range);
    } else if (gGui->setup()->editingPeaks()) {
        ; // do nothing
    }
}

bool DiffractogramPlotOverlay::addModeColor(QColor& color) const
{
    if        (gGui->setup()->editingBaseline()) {
        color = {0x98, 0xfb, 0x98, 0x70}; // medium green
        return true;
    } else if (gGui->setup()->editingPeaks()) {
        color = {0x87, 0xce, 0xfa, 0x70}; // medium blue
        return true;
    }
    return false;
}

bool DiffractogramPlotOverlay::subtractModeColor(QColor& color) const
{
    if        (gGui->setup()->editingBaseline()) {
        color = {0xf8, 0xf8, 0xff, 0x90}; // almost white
        return true;
    }
    return false;
}


// ************************************************************************** //
//  class DiffractogramPlot
// ************************************************************************** //

DiffractogramPlot::DiffractogramPlot(Diffractogram& diffractogram)
    : diffractogram_(diffractogram)
    , showBgFit_(false)
    , overlay_(new DiffractogramPlotOverlay(*this))
{
    QCPAxisRect* ar = axisRect();

    // fix margins
    QFontMetrics fontMetrics(font());
    int em = fontMetrics.width('M');
    int ascent = fontMetrics.ascent();
    QMargins margins(6 * em, ascent, em, 2 * ascent);
    ar->setAutoMargins(QCP::msNone);
    ar->setMargins(margins);
    overlay_->setMargins(margins.left(), margins.right());

    // colours
    setBackground(palette().background().color());
    ar->setBackground(Qt::white);

    // graphs in the "main" layer; in the display order
    bgGraph_ = addGraph();
    bgGraph_->setPen(QPen(QColor(0x21, 0xa1, 0x21, 0xff), 2));

    dgramGraph_ = addGraph();
    dgramGraph_->setLineStyle(QCPGraph::LineStyle::lsNone);
    dgramGraph_->setScatterStyle(
        QCPScatterStyle(QCPScatterStyle::ScatterShape::ssDisc, Qt::gray, 2));

    dgramBgFittedGraph2_ = addGraph();
    dgramBgFittedGraph2_->setVisible(false);
    dgramBgFittedGraph2_->setLineStyle(QCPGraph::LineStyle::lsNone);
    dgramBgFittedGraph2_->setScatterStyle(
        QCPScatterStyle(QCPScatterStyle::ScatterShape::ssDisc, QColor(255, 0, 0), 4));

    dgramBgFittedGraph_ = addGraph();
    dgramBgFittedGraph_->setPen(QPen(Qt::black, 2));

    // background layers
    addLayer("bg", layer("baseline"), QCustomPlot::limAbove);
    addLayer("refl", layer("main"), QCustomPlot::limAbove);
    addLayer("marks", layer("refl"), QCustomPlot::limAbove);
    setCurrentLayer("marks");

    guesses_ = addGraph();
    guesses_->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle, 8));
    guesses_->setLineStyle(QCPGraph::lsNone);
    guesses_->setPen(QPen(Qt::darkGray));

    fits_ = addGraph();
    fits_->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle, 8));
    fits_->setLineStyle(QCPGraph::lsNone);
    fits_->setPen(QPen(Qt::red));

    connect(&gGui->toggles->showBackground, &QAction::toggled, [this](bool on) {
        showBgFit_ = on;
        renderAll();
    });

    connect(gSession, &Session::sigPeaks, this, &DiffractogramPlot::renderAll);
    // indirect: connect(gSession, &Session::sigActivated, this, &DiffractogramPlot::renderAll);
    // indirect: connect(gSession, &Session::sigDetector, this, &DiffractogramPlot::renderAll);
    connect(gSession, &Session::sigDiffractogram, this, &DiffractogramPlot::renderAll);
}

void DiffractogramPlot::clearReflLayer()
{
    for (QCPGraph* g : reflGraph_)
        removeGraph(g);
    reflGraph_.clear();
}

void DiffractogramPlot::enterZoom(bool on)
{
    overlay_->setHidden(on);
    dgramBgFittedGraph2_->setVisible(on);
}

//! Paints a colored rectangle in the background layer, to indicate area of baseline or peak fit
void DiffractogramPlot::addBgItem(const Range& range, const QColor& color)
{
    setCurrentLayer("bg");
    QCPItemRect* ir = new QCPItemRect(this);
    ir->setPen(QPen(color));
    ir->setBrush(QBrush(color));
    QCPItemPosition* br = ir->bottomRight;
    br->setTypeY(QCPItemPosition::ptViewportRatio);
    br->setCoords(range.max, 1);
    QCPItemPosition* tl = ir->topLeft;
    tl->setTypeY(QCPItemPosition::ptViewportRatio);
    tl->setCoords(range.min, 0);
    addItem(ir);
}

void DiffractogramPlot::resizeEvent(QResizeEvent* e)
{
    QCustomPlot::resizeEvent(e);
    const QSize size = e->size();
    overlay_->setGeometry(0, 0, size.width(), size.height());
}

void DiffractogramPlot::onPeakData()
{
    Peak* peak = gSession->peaks().selectedPeak();
    guesses_->clearData();
    fits_->clearData();

    if (peak && gSession->dataset().highlight().cluster()) {
        const PeakFunction& fun = peak->peakFunction();

        const qpair gp = fun.guessedPeak();
        if (gp.isValid()) {
            guesses_->addData(gp.x, gp.y);
            qreal gw2 = fun.guessedFWHM() / 2;
            guesses_->addData(gp.x - gw2, gp.y / 2);
            guesses_->addData(gp.x + gw2, gp.y / 2);
        }

        const qpair fp = fun.fittedPeak();
        if (fp.isValid()) {
            fits_->addData(fp.x, fp.y);
            qreal fw2 = fun.fittedFWHM() / 2;
            fits_->addData(fp.x - fw2, fp.y / 2);
            fits_->addData(fp.x + fw2, fp.y / 2);
        }
    }
    renderAll();
}

//! Repaints everything, including the colored background areas.
void DiffractogramPlot::renderAll()
{
    clearItems();

    const Ranges& rs = gSession->baseline().ranges();
    for_i (rs.count())
        addBgItem(rs.at(i), {0x98, 0xfb, 0x98, 0x50}); // light green
    for_i (gSession->peaks().count()) {
        addBgItem(gSession->peaks().at(i).range(), i==gSession->peaks().selectedIndex() ?
                  QColor(0x87, 0xce, 0xfa, 0x70) : // medium blue
                  QColor(0x87, 0xce, 0xfa, 0x50)); // light blue
    }

    if (!gSession->dataset().highlight().cluster()) {
        plotEmpty();
        return;
    }
    calcDgram();
    calcBackground();
    calcPeaks();
    plot(dgram_, dgramBgFitted_, bg_, refls_, currReflIndex_);
}

void DiffractogramPlot::calcDgram()
{
    dgram_.clear();
    if (!gSession->hasData())
        return;
    if (gGui->toggles->combinedDgram.isChecked()) {
        dgram_ = gSession->activeClusters().avgCurve();
    } else {
        dgram_ = gSession->dataset().highlight().cluster()->toCurve(
            gSession->gammaSelection().range());
    }
}

void DiffractogramPlot::calcBackground()
{
    bg_.clear();
    dgramBgFitted_.clear();

    const Polynom& bgPolynom = Polynom::fromFit(
        gSession->baseline().polynomDegree(), dgram_, gSession->baseline().ranges());
        // TODO bundle this code line which similarly appears in at least one other place

    for_i (dgram_.count()) {
        qreal x = dgram_.x(i), y = bgPolynom.y(x);
        bg_.append(x, y);
        dgramBgFitted_.append(x, dgram_.y(i) - y);
    }
}

void DiffractogramPlot::calcPeaks()
{
    refls_.clear();
    currReflIndex_ = 0;

    for_i (gSession->peaks().count()) {
        Peak& r = gSession->peaks().at(i);
        if (&r == gSession->peaks().selectedPeak())
            currReflIndex_ = i;

        r.fit(dgramBgFitted_);

        const Range& rge = r.range();
        const PeakFunction& fun = r.peakFunction();

        Curve c;
        for_i (dgramBgFitted_.count()) {
            qreal x = dgramBgFitted_.x(i);
            if (rge.contains(x))
                c.append(x, fun.y(x));
        }
        refls_.append(c);
    }
}

void DiffractogramPlot::plot(
    const Curve& dgram, const Curve& dgramBgFitted, const Curve& bg, const curve_vec& refls,
    int currReflIndex)
{
    if (dgram.isEmpty()) {
        plotEmpty();
        return;
    }
    const Range& tthRange = dgram.rgeX();

    Range intenRange;
    if (gGui->toggles->fixedIntenDgram.isChecked()) {
        intenRange = gSession->dataset().highlight().cluster()->rgeInten();
    } else {
        intenRange = dgramBgFitted.rgeY();
        intenRange.extendBy(dgram.rgeY());
    }

    xAxis->setRange(tthRange.min, tthRange.max);
    yAxis->setRange(qMin(0., intenRange.min), intenRange.max);
    yAxis->setNumberFormat("g");
    xAxis->setVisible(true);
    yAxis->setVisible(true);

    if (showBgFit_) {
        bgGraph_->setData(bg.xs(), bg.ys());
    } else {
        bgGraph_->clearData();
    }

    dgramGraph_->setData(dgram.xs(), dgram.ys());
    dgramBgFittedGraph_->setData(dgramBgFitted.xs(), dgramBgFitted.ys());
    dgramBgFittedGraph2_->setData(dgramBgFitted.xs(), dgramBgFitted.ys());

    clearReflLayer();
    setCurrentLayer("refl");

    for_i (refls.count()) {
        const Curve& r = refls.at(i);
        QCPGraph* graph = addGraph();
        reflGraph_.append(graph);
        graph->setPen(QPen(Qt::green, i == currReflIndex ? 2 : 1));
        graph->setData(r.xs(), r.ys());
    }

    replot();
}

void DiffractogramPlot::plotEmpty()
{
    xAxis->setVisible(false);
    yAxis->setVisible(false);

    bgGraph_->clearData();
    dgramGraph_->clearData();
    dgramBgFittedGraph_->clearData();
    dgramBgFittedGraph2_->clearData();

    clearReflLayer();
    replot();
}
