//  ***********************************************************************************************
//
//  Steca: stress and texture calculator
//
//! @file      gui/view/plot_dfgram.cpp
//! @brief     Implements class PlotDfgram. Has local class PlotDfgramOverlay.
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
//  ***********************************************************************************************

#include "gui/view/plot_dfgram.h"
#include "core/session.h"
#include "gui/view/toggles.h"
#include "gui/mainwin.h"
#include "gui/view/plot_overlay.h"
#include "gui/view/range_control.h"
#include "qcr/base/qcrexception.h"

namespace colors {
QColor baseEmph{0x00, 0xff, 0x00, 0x50}; // green
QColor baseStd {0x87, 0xce, 0x87, 0x50}; // light green
QColor baseEdit{0x00, 0xff, 0x00, 0x30}; // as Emph, but more transparent
QColor peakEdit{0x00, 0xff, 0xff, 0x50}; // cyan
QColor peakStd {0x87, 0xce, 0xfa, 0x50}; // light blue
QColor peakFit {0x00, 0x00, 0xff, 0xff}; // blue
QColor pen     {0x21, 0xa1, 0x21, 0xff};
QColor scatter {255, 0, 0};
}

//  ***********************************************************************************************
//! @class PlotDfgramOverlay

//! Listens to mouse events to select subranges of a PlotDfgram.

//! Equips PlotOverlay with domain-specific colors and setter functions.

class PlotDfgramOverlay : public PlotOverlay, public QcrSettable {
public:
    PlotDfgramOverlay(class PlotDfgram& parent)
        : PlotOverlay{parent, RangeControl::STEP}, QcrSettable{*this,"dfgram"} {}
private:
    void executeConsoleCommand(const QString&) final;
    void addRange(const Range&) final;
    void selectRange(double x) final;
    const QColor* mousedColor() const final;
};


void PlotDfgramOverlay::addRange(const Range& range)
{
    doLog(QString("dfgram add %1 %2").arg(range.min).arg(range.max));
    switch (gSession->params.editableRange) {
    case EditableRange::BASELINE:
        gSession->baseline.ranges.add(range);
        gSession->onBaseline();
        break;
    case EditableRange::PEAKS:
        gSession->peaks.add(range);
        gSession->onPeaks();
        break;
    default:
        return;
    }
    gRoot->remakeAll();
}

//! Selects the range that contains pixel x.

void PlotDfgramOverlay::selectRange(double x)
{
    doLog(QString("dfgram sel %1").arg(x));
    bool selectionChanged = false;
    // prioritize baseline sel. when editing baselines
    if (gSession->params.editableRange == EditableRange::BASELINE)
        selectionChanged = gSession->baseline.ranges.selectByValue(x);
    if (!selectionChanged) // no baseline found:
        selectionChanged = gSession->peaks.selectByValue(x);
    //if a peak or a baseline.range has been selected, redraw all:
    if (selectionChanged)
        gRoot->remakeAll();
}

void PlotDfgramOverlay::executeConsoleCommand(const QString& arg)
{
    QStringList args = arg.split(' ');
    if (args[0]=="add") {
        if (args.size()<3)
            throw QcrException("Missing arguments to command 'add'");
        addRange(Range(strOp::to_d(args[1]), strOp::to_d(args[2])));
    } else if (args[0]=="sel") {
        if (args.size()<2)
            throw QcrException("Missing argument to command 'sel'");
        selectRange(strOp::to_d(args[1]));
    } else
        throw QcrException("Unexpected dfgram command");
}

//! Returns color to be used when the mouse is marking a range.

const QColor* PlotDfgramOverlay::mousedColor() const
{
    switch (gSession->params.editableRange) {
    case EditableRange::BASELINE:
        return &colors::baseEdit;
    case EditableRange::PEAKS:
        return &colors::peakEdit;
    default:
        return nullptr;
    }
}

//  ***********************************************************************************************
//! @class PlotDfgram

PlotDfgram::PlotDfgram()
    : overlay_(new PlotDfgramOverlay(*this))
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

//! Repaints everything, including the colored background areas.
void PlotDfgram::renderAll()
{
    clearItems();

    // Render colored background areas to indicate baseline and peak fit ranges.
    const Ranges& ranges = gSession->baseline.ranges;
    bool showBaselineHighlight = gSession->params.editableRange == EditableRange::BASELINE;
    bool showPeakHighlight = true;
    for (int jR=0; jR<ranges.size(); ++jR)
        addBgItem(ranges.at(jR),
                  showBaselineHighlight && jR==gSession->baseline.ranges.selectedIndex() ?
                  colors::baseEmph : colors::baseStd);
    for (int jP=0; jP<gSession->peaks.size(); ++jP)
        addBgItem(gSession->peaks.at(jP).range(),
                  showPeakHighlight && jP==gSession->peaks.selectedIndex() ?
                  colors::peakEdit : colors::peakStd);

    if (!gSession->hasData() || !gSession->currentCluster()) {
        plotEmpty();
        return;
    }

    const Dfgram* dfgram = gSession->currentOrAvgeDfgram();
    ASSERT(!dfgram->curve.isEmpty());

    // retrieve background
    const Curve& bg           = dfgram->getBgAsCurve();
    const Curve& curveMinusBg = dfgram->getCurveMinusBg();

    // calculate peaks
    std::vector<Curve> fitCurves;
    for (int jP=0; jP<gSession->peaks.size(); ++jP)
        fitCurves.push_back(dfgram->getPeakAsCurve(jP));

    const Range& tthRange = dfgram->curve.rgeX();
    Range intenRange;
    if (gGui->toggles->fixedIntenDfgram.getValue()) {
        intenRange = gSession->currentCluster()->rgeInten();
    } else {
        intenRange = curveMinusBg.rgeY();
        intenRange.extendBy(dfgram->curve.rgeY());
    }

    xAxis->setRange(tthRange.min, tthRange.max);
    yAxis->setRange(qMin(0., intenRange.min), intenRange.max);
    yAxis->setNumberFormat("g");
    xAxis->setVisible(true);
    yAxis->setVisible(true);

    if (gGui->toggles->showBackground.getValue() && !bg.isEmpty())
        bgGraph_->setData(QVector<double>::fromStdVector(bg.xs()),
                          QVector<double>::fromStdVector(bg.ys()));
    else
        bgGraph_->clearData();

    dgramGraph_->setData(QVector<double>::fromStdVector(dfgram->curve.xs()),
                         QVector<double>::fromStdVector(dfgram->curve.ys()));
    dgramBgFittedGraph_->setData(QVector<double>::fromStdVector(curveMinusBg.xs()),
                                 QVector<double>::fromStdVector(curveMinusBg.ys()));
    dgramBgFittedGraph2_->setData(QVector<double>::fromStdVector(curveMinusBg.xs()),
                                  QVector<double>::fromStdVector(curveMinusBg.ys()));

    clearReflLayer();
    setCurrentLayer("refl");

    for (int jP=0; jP<fitCurves.size(); ++jP) {
        const Curve& r = fitCurves.at(jP);
        QCPGraph* graph = addGraph();
        reflGraph_.push_back(graph);
        graph->setPen(QPen(colors::peakFit, 2));
        graph->setData(QVector<double>::fromStdVector(r.xs()),
                       QVector<double>::fromStdVector(r.ys()));
    }

    replot();
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
