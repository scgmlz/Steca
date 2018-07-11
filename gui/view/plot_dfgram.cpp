//  ***********************************************************************************************
//
//  Steca: stress and texture calculator
//
//! @file      gui/view/plot_dfgram.cpp
//! @brief     Implements classes PlotDfgram and Dfgram.
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
//  ***********************************************************************************************

#include "plot_dfgram.h"
#include "core/algo/collect_intensities.h"
#include "core/def/idiomatic_for.h"
#include "core/session.h"
#include "gui/actions/toggles.h"
#include "gui/panels/subframe_setup.h"
#include "gui/state.h"
#include "qcr/engine/console.h"

namespace colors {
QColor baseEmph{0x00, 0xff, 0x00, 0x50}; // green
QColor baseStd {0x87, 0xce, 0x87, 0x50}; // light green
QColor baseEdit{0x00, 0xff, 0x00, 0x30}; // as Emph, but more transparent
QColor peakEmph{0x00, 0xff, 0xff, 0x50}; // cyan
QColor peakStd {0x87, 0xce, 0xfa, 0x50}; // light blue
QColor peakEdit{0x00, 0xff, 0xff, 0x30}; // as Emph, but more transparent
QColor subtract{0xf8, 0xf8, 0xff, 0x90}; // almost white
QColor pen     {0x21, 0xa1, 0x21, 0xff};
QColor scatter {255, 0, 0};
}

//  ***********************************************************************************************
//! @class PlotDfgramOverlay

PlotDfgramOverlay::PlotDfgramOverlay(PlotDfgram& parent)
    : PlotOverlay(parent)
{}

void PlotDfgramOverlay::addRange(const Range& range)
{
    if        (gGui->state->editingBaseline) {
        gSession->baseline().addRange(range);
    } else if (gGui->state->editingPeakfits) {
        if (Peak* peak = gSession->peaks().selectedPeak()) {
            peak->setRange(range);
            gConsole->log("peakRangeMin "+QString::number(range.min));
            gConsole->log("peakRangeMax "+QString::number(range.max));
        }
    }
    gRoot->remakeAll("PlotDfgramOverlay::addRange");
}

bool PlotDfgramOverlay::addModeColor(QColor& color) const
{
    if        (gGui->state->editingBaseline) {
        color = colors::baseEdit;
        return true;
    } else if (gGui->state->editingPeakfits) {
        color = colors::peakEdit;
        return true;
    }
    return false;
}

bool PlotDfgramOverlay::subtractModeColor(QColor& color) const
{
    if        (gGui->state->editingBaseline) {
        color = colors::subtract;
        return true;
    }
    return false;
}


//  ***********************************************************************************************
//! @class PlotDfgram

PlotDfgram::PlotDfgram(Dfgram& diffractogram)
    : diffractogram_(diffractogram)
    , overlay_(new PlotDfgramOverlay(*this))
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
    bgGraph_->setPen(QPen(colors::pen, 2));

    dgramGraph_ = addGraph();
    dgramGraph_->setLineStyle(QCPGraph::LineStyle::lsNone);
    dgramGraph_->setScatterStyle(
        QCPScatterStyle(QCPScatterStyle::ScatterShape::ssDisc, Qt::gray, 2));

    dgramBgFittedGraph2_ = addGraph();
    dgramBgFittedGraph2_->setVisible(false);
    dgramBgFittedGraph2_->setLineStyle(QCPGraph::LineStyle::lsNone);
    dgramBgFittedGraph2_->setScatterStyle(
        QCPScatterStyle(QCPScatterStyle::ScatterShape::ssDisc, colors::scatter, 4));

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
}

void PlotDfgram::clearReflLayer()
{
    for (QCPGraph* g : reflGraph_)
        removeGraph(g);
    reflGraph_.clear();
}

void PlotDfgram::enterZoom(bool on)
{
    overlay_->setHidden(on);
    dgramBgFittedGraph2_->setVisible(on);
}

//! Paints a colored rectangle in the background layer, to indicate area of baseline or peak fit
void PlotDfgram::addBgItem(const Range& range, const QColor& color)
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

void PlotDfgram::resizeEvent(QResizeEvent* e)
{
    QCustomPlot::resizeEvent(e);
    const QSize size = e->size();
    overlay_->setGeometry(0, 0, size.width(), size.height());
}

void PlotDfgram::onPeakData()
{
    Peak* peak = gSession->peaks().selectedPeak();
    guesses_->clearData();
    fits_->clearData();

    if (peak && gSession->dataset().highlight().cluster()) {
        const PeakFunction& fun = peak->peakFunction();

        const qpair gp = fun.guessedPeak();
        if (gp.isValid()) {
            guesses_->addData(gp.x, gp.y);
            double gw2 = fun.guessedFWHM() / 2;
            guesses_->addData(gp.x - gw2, gp.y / 2);
            guesses_->addData(gp.x + gw2, gp.y / 2);
        }

        const qpair fp = fun.fittedPeak();
        if (fp.isValid()) {
            fits_->addData(fp.x, fp.y);
            double fw2 = fun.fittedFWHM() / 2;
            fits_->addData(fp.x - fw2, fp.y / 2);
            fits_->addData(fp.x + fw2, fp.y / 2);
        }
    }
    renderAll();
}

//! Repaints everything, including the colored background areas.
void PlotDfgram::renderAll()
{
    clearItems();

    const Ranges& rs = gSession->baseline().ranges();
    // qDebug() << "PlotDfgram::renderAll #bg-ranges=" << rs.count();
    bool showingBaseline = gGui->setup()->currentIndex() == gGui->setup()->idxBaseline;
    for_i (rs.count())
        addBgItem(rs.at(i),
                  showingBaseline && i==gSession->baseline().selectedIndex() ?
                  colors::baseEmph : colors::baseStd);
    for_i (gSession->peaks().count())
        addBgItem(gSession->peaks().at(i).range(),
                  !showingBaseline && i==gSession->peaks().selectedIndex() ?
                  colors::peakEmph : colors::peakStd);

    if (!gSession->dataset().highlight().cluster()) {
        plotEmpty();
        return;
    }
    calcDfgram();
    if (dgram_.isEmpty()) {
        plotEmpty();
        return;
    }
    calcBackground();
    calcPeaks();

    const Range& tthRange = dgram_.rgeX();
    Range intenRange;
    if (gGui->toggles->fixedIntenDfgram.getValue()) {
        intenRange = gSession->dataset().highlight().cluster()->rgeInten();
    } else {
        intenRange = dgramBgFitted_.rgeY();
        intenRange.extendBy(dgram_.rgeY());
    }

    xAxis->setRange(tthRange.min, tthRange.max);
    yAxis->setRange(qMin(0., intenRange.min), intenRange.max);
    yAxis->setNumberFormat("g");
    xAxis->setVisible(true);
    yAxis->setVisible(true);

    if (gGui->toggles->showBackground.getValue())
        bgGraph_->setData(QVector<double>::fromStdVector(bg_.xs()),
                          QVector<double>::fromStdVector(bg_.ys()));
    else
        bgGraph_->clearData();

    dgramGraph_->setData(QVector<double>::fromStdVector(dgram_.xs()),
                         QVector<double>::fromStdVector(dgram_.ys()));
    dgramBgFittedGraph_->setData(QVector<double>::fromStdVector(dgramBgFitted_.xs()),
                                 QVector<double>::fromStdVector(dgramBgFitted_.ys()));
    dgramBgFittedGraph2_->setData(QVector<double>::fromStdVector(dgramBgFitted_.xs()),
                                  QVector<double>::fromStdVector(dgramBgFitted_.ys()));

    clearReflLayer();
    setCurrentLayer("refl");

    for_i (refls_.size()) {
        const Curve& r = refls_.at(i);
        QCPGraph* graph = addGraph();
        reflGraph_.push_back(graph);
        graph->setPen(QPen(Qt::green, i == currReflIndex_ ? 2 : 1));
        graph->setData(QVector<double>::fromStdVector(r.xs()),
                       QVector<double>::fromStdVector(r.ys()));
    }

    replot();
}

void PlotDfgram::calcDfgram()
{
    dgram_.clear();
    if (!gSession->hasData())
        return;
    if (gGui->toggles->combinedDfgram.getValue())
        dgram_ = gSession->activeClusters().avgCurve();
    else
        dgram_ = algo::projectCluster(*gSession->dataset().highlight().cluster(),
                                      gSession->gammaSelection().range());
}

void PlotDfgram::calcBackground()
{
    bg_.clear();
    dgramBgFitted_.clear();

    const Polynom& bgPolynom = Polynom::fromFit(
        gSession->baseline().polynomDegree.val(), dgram_, gSession->baseline().ranges());
        // TODO bundle this code line which similarly appears in at least one other place

    for_i (dgram_.count()) {
        double x = dgram_.x(i), y = bgPolynom.y(x);
        bg_.append(x, y);
        dgramBgFitted_.append(x, dgram_.y(i) - y);
    }
}

void PlotDfgram::calcPeaks()
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
            double x = dgramBgFitted_.x(i);
            if (rge.contains(x))
                c.append(x, fun.y(x));
        }
        refls_.push_back(c);
    }
}

void PlotDfgram::plotEmpty()
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
