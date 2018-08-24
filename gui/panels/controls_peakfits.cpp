//  ***********************************************************************************************
//
//  Steca: stress and texture calculator
//
//! @file      gui/panels/controls_peakfits.cpp
//! @brief     Implements class ControlsPeakfits, and local classes
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
//  ***********************************************************************************************

#include "gui/panels/controls_peakfits.h"
#include "core/session.h"
#include "core/fit/peak_function.h"
#include "qcr/widgets/tables.h"
#include "gui/mainwin.h"
#include "gui/actions/triggers.h"
#include "gui/view/range_control.h"
//#include "qcr/base/debug.h"

namespace {

QString safeRealText(double val, int prec=4) {
    return qIsFinite(val) ? QString::number(val, 'g', prec) : "NaN"; }
QString par2text(const DoubleWithError& par) {
    return safeRealText(par.value,4) + "+-" + safeRealText(par.roundedError(4),4); }

} // namespace

//  ***********************************************************************************************
//! @class PeaksModel (local scope)

//! Model for table of peaks.

class PeaksModel : public TableModel {
public:
    PeaksModel() : TableModel("peaks") {}

    int columnCount() const final { return NUM_COLUMNS; }
    int rowCount() const final { return gSession->peaks.size(); }
    int highlighted() const final { return gSession->peaks.selectedIndex(); }
    void onHighlight(int row) final { gSession->peaks.select(row); }

    QVariant data(const QModelIndex&, int) const;

    enum { COL_ID = 1, COL_TYPE, COL_RANGE, NUM_COLUMNS };
};

QVariant PeaksModel::data(const QModelIndex& index, int role) const
{
    int row = index.row();
    if (row < 0 || rowCount() <= row)
        return {};
    const Peak& peak = gSession->peaks.at(row);
    switch (role) {
    case Qt::DisplayRole: {
        int col = index.column();
        if (col < 1)
            return {};
        switch (col) {
        case COL_ID:
            return QString::number(row + 1);
        case COL_TYPE:
            return peak.functionName();
        case COL_RANGE:
            if (!peak.range().isValid())
                return "undefined";
            else if (peak.range().isEmpty())
                return "empty";
            return peak.range().to_s();
        default:
            return {};
        }
    }
    case Qt::ForegroundRole: {
        if (peak.range().isEmpty())
            return QColor(Qt::red);
        return QColor(Qt::black);
    }
    case Qt::BackgroundRole: {
        if (row==highlighted())
            return QColor(Qt::cyan);
        return QColor(Qt::white);
    }
    default:
        return {};
    }
}


//  ***********************************************************************************************
//! @class PeakfitOutcomeView (local scope)

//! Displays outcome of raw analysis and fit of current peak for current dfgram.

class PeakfitOutcomeView : public QcrWidget {
public:
    PeakfitOutcomeView();
private:
    void refresh();
    void enable(bool haveRaw, bool haveFit);
    QcrLineDisplay showFittedX_ {10, true};
    QcrLineDisplay showFittedD_ {10, true};
    QcrLineDisplay showFittedY_ {10, true};
    QcrLineDisplay showRawOutcomeX_ { 5, true};
    QcrLineDisplay showRawOutcomeY_ { 5, true};
    QcrLineDisplay showRawOutcomeD_ { 5, true};
};

PeakfitOutcomeView::PeakfitOutcomeView()
{
    auto* grid = new QGridLayout;
    grid->addWidget(new QLabel("direct"), 0, 1);
    grid->addWidget(new QLabel("fitted"), 0, 2);

    grid->addWidget(new QLabel("centre"), 1, 0);
    grid->addWidget(&showRawOutcomeX_, 1, 1);
    grid->addWidget(&showFittedX_, 1, 2);
    grid->addWidget(new QLabel("deg"), 1, 3);

    grid->addWidget(new QLabel("fwhm"), 2, 0);
    grid->addWidget(&showRawOutcomeD_, 2, 1);
    grid->addWidget(&showFittedD_, 2, 2);
    grid->addWidget(new QLabel("deg"), 2, 3);

    grid->addWidget(new QLabel("intens"), 3, 0);
    grid->addWidget(&showRawOutcomeY_, 3, 1);
    grid->addWidget(&showFittedY_, 3, 2);

    grid->setColumnStretch(4, 1);
    setLayout(grid);

    setRemake([this](){ refresh(); });
}

void PeakfitOutcomeView::refresh()
{
    const Peak* peak = gSession->peaks.selectedPeak();
    if (!peak)
        return enable(false, false);

    const Dfgram* dfgram = gSession->currentOrAvgeDfgram();
    ASSERT(dfgram); // the entire tab should be disabled if there is no active cluster

    int jP = gSession->peaks.selectedIndex();
    const RawOutcome& outcome = dfgram->getRawOutcome(jP);
    showRawOutcomeX_.setText(safeRealText(outcome.getCenter()));
    showRawOutcomeD_.setText(safeRealText(outcome.getFwhm()));
    showRawOutcomeY_.setText(safeRealText(outcome.getIntensity()));

    if (peak->isRaw())
        return enable(true, false);
    const Fitted& pFct = dfgram->getPeakFit(jP);
    const auto* peakFit = dynamic_cast<const PeakFunction*>(pFct.f);
    ASSERT(peakFit);
    const PeakOutcome out = peakFit->outcome(pFct);
    showFittedX_.setText(par2text(out.center));
    showFittedD_.setText(par2text(out.fwhm));
    showFittedY_.setText(par2text(out.intensity));
    enable(true, true);
}

void PeakfitOutcomeView::enable(bool haveRaw, bool haveFit)
{
    showRawOutcomeX_.setEnabled(haveRaw);
    showRawOutcomeD_.setEnabled(haveRaw);
    showRawOutcomeY_.setEnabled(haveRaw);
    if (!haveRaw) {
        showRawOutcomeX_.setText("");
        showRawOutcomeD_.setText("");
        showRawOutcomeY_.setText("");
    }

    showFittedX_.setEnabled(haveFit);
    showFittedD_.setEnabled(haveFit);
    showFittedY_.setEnabled(haveFit);
    if (!haveFit) {
        showFittedX_.setText("");
        showFittedD_.setText("");
        showFittedY_.setText("");
    }
}

//  ***********************************************************************************************
//! @class ControlsPeakfits

ControlsPeakfits::ControlsPeakfits()
{
    auto* comboPeakFct = new QcrComboBox{
        "reflTyp", &gSession->params.defaultPeakFunction,
        []()->QStringList{return Peak::keys;} };
    comboPeakFct->setHook([](int i){
            const QString& name = Peak::keys[i];
            if (Peak* p = gSession->peaks.selectedPeak())
                p->setPeakFunction(name);
            gSession->onPeaks(); });

    auto* box = new QVBoxLayout;

    auto* topControls = new QHBoxLayout;
    topControls->addStretch();
    topControls->addWidget(new QcrIconTriggerButton(&gGui->triggers->peakAdd));
    topControls->addWidget(new QcrIconTriggerButton(&gGui->triggers->peakRemove));
    topControls->addWidget(new QcrIconTriggerButton(&gGui->triggers->peaksClear));
    box->addLayout(topControls);

    box->addWidget(new TableView(new PeaksModel()));
    box->addWidget(comboPeakFct);
    box->addWidget(new RangeControl("peak",
                                    []()->const Range* {
                                        const Peak* p = gSession->peaks.selectedPeak();
                                        return p ? &p->range() : nullptr; },
                                    [](double val, bool namelyMax){
                                        Peak* p = gSession->peaks.selectedPeak();
                                        ASSERT(p);
                                        p->setOne(val, namelyMax);
                                        gSession->onPeaks(); }
                       ));
    box->addWidget(new PeakfitOutcomeView);
    box->addStretch(1000);
    setLayout(box);
}
