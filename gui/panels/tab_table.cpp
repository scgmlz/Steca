// ************************************************************************** //
//
//  Steca: stress and texture calculator
//
//! @file      gui/panels/tab_table.cpp
//! @brief     Implements class TableWidget, and its dependences
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
// ************************************************************************** //

#include "tab_table.h"
#include "core/session.h"
#include "gui/actions/toggles.h"
#include "gui/actions/triggers.h"
#include "gui/dialogs/data_table.h"
#include <QScrollArea>
#include <QThread> // for sleep for debugging
#include <iostream>

struct showcol_t {
    QString name;
    CCheckBox* cb;
};

typedef vec<showcol_t> showcol_vec;

// ************************************************************************** //
//  local class ColumnSelector
// ************************************************************************** //

//! A row of controls for choosing which data columns are to be displayed in a TabTable.

class ColumnSelector : public QWidget {
public:
    ColumnSelector(DataView&, showcol_vec&);
private:
    DataView& table_;
    showcol_vec& showCols_;
    QVBoxLayout box_;
    CRadioButton rbHidden_, rbAll_, rbNone_, rbInten_, rbTth_, rbFWHM_;
};

ColumnSelector::ColumnSelector(DataView& table, showcol_vec& showCols)
    : table_(table)
    , showCols_(showCols)
    , rbHidden_("rbHidden", "")
    , rbAll_("rbAll", "all")
    , rbNone_("rbNone", "none")
    , rbInten_("rbInten", "Intensity")
    , rbTth_("rbTth", "2θ")
    , rbFWHM_("rbFWHM", "fwhm")
{
    using eReflAttr = PeakInfo::eReflAttr;

    setLayout(&box_);

    box_.addWidget(&rbHidden_);
    rbHidden_.hide();

    box_.addWidget(&rbAll_);
    box_.addWidget(&rbNone_);
    box_.addWidget(&rbInten_);
    box_.addWidget(&rbTth_);
    box_.addWidget(&rbFWHM_);
    box_.addSpacing(8);

    for_i (showCols.count()) {
        showcol_t& item = showCols[i];
        box_.addWidget((item.cb = new CCheckBox("cb"+QString::number(i), item.name)));
    }

    auto _all = [this]() {
        for (showcol_t& col : showCols_)
            col.cb->setChecked(true);
    };

    auto _none = [this]() {
        for (showcol_t& col : showCols_)
            col.cb->setChecked(false);
    };

    auto _showInten = [this, _none]() {
        _none();
        showCols_.at(int(eReflAttr::INTEN)).cb->setChecked(true);
    };

    auto _showTth = [this, _none]() {
        _none();
        showCols_.at(int(eReflAttr::TTH)).cb->setChecked(true);
    };

    auto _showFWHM = [this, _none]() {
        _none();
        showCols_.at(int(eReflAttr::FWHM)).cb->setChecked(true);
    };

    auto _updateRbs = [this]() {
        bool isAll = true, isNone = true, isOther = false;
        int nInten = 0, nTth = 0, nFwhm = 0;

        for_i (showCols_.count()) {
            if (!showCols_.at(i).cb->isChecked()) {
                isAll = false;
                continue;
            }

            isNone = false;

            switch (eReflAttr(i)) {
            case eReflAttr::ALPHA:
            case eReflAttr::BETA:
                ++nInten;
                ++nTth;
                ++nFwhm;
                break;
            case eReflAttr::INTEN: ++nInten; break;
            case eReflAttr::TTH: ++nTth; break;
            case eReflAttr::FWHM: ++nFwhm; break;
            default: isOther = true; break;
            }
        }

        rbHidden_.setChecked(true);
        rbNone_.setChecked(isNone);
        rbAll_.setChecked(isAll);

        int const PRESET_SELECTION = 1;

        rbInten_.setChecked(!isOther && PRESET_SELECTION == nInten);
        rbTth_.setChecked(!isOther && PRESET_SELECTION == nTth);
        rbFWHM_.setChecked(!isOther && PRESET_SELECTION == nFwhm);
    };

    for_i (showCols_.count()) {
        QCheckBox* cb = showCols_.at(i).cb;
        connect(cb, &QCheckBox::toggled, [this, _updateRbs, i](bool on) {
            if (on)
                table_.showColumn(i + 1);
            else
                table_.hideColumn(i + 1);

            _updateRbs();
        });
    }

    connect(&rbAll_, &QRadioButton::clicked, _all);
    connect(&rbNone_, &QRadioButton::clicked, _none);
    connect(&rbInten_, &QRadioButton::clicked, _showInten);
    connect(&rbTth_, &QRadioButton::clicked, _showTth);
    connect(&rbFWHM_, &QRadioButton::clicked, _showFWHM);

    rbAll_.click();
}


// ************************************************************************** //
//  class TableWidget
// ************************************************************************** //

TableWidget::TableWidget()
{
    // inbound connection
    connect(gSession, &Session::sigPeaks, [this]() {
            QThread::msleep(3000); });
            // calculate(); });

    // business logic // TODO: move elsewhere
    const QStringList& headers = PeakInfo::dataTags(false);
    const QStringList& outHeaders = PeakInfo::dataTags(true);
    const cmp_vec& cmps =PeakInfo::dataCmps();
    auto* showCols = new showcol_vec;
    for_i (headers.count()) {
        showcol_t item;
        item.name = headers.at(i);
        showCols->append(item);
    }

    // layout
    auto* layout = new QHBoxLayout;

    dataView_ = new DataView(headers.count()); // the main table
    layout->addWidget(dataView_);

    auto* scrollArea = new QScrollArea;
    scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    scrollArea->setWidget(new ColumnSelector(*dataView_, *showCols));
    layout->addWidget(scrollArea);

    layout->setStretch(0,1000);
    setLayout(layout);
}

void TableWidget::calculate()
{
    TakesLongTime __;

    calcPoints_.clear();
    interpPoints_.clear();

    int reflCount = gSession->peaks().count();
    if (!reflCount)
        return;

    Progress progress(reflCount, &gGui->progressBar);

    for_i (reflCount)
        calcPoints_.append(
            gSession->makePeakInfos(
                gSession->peaks().at(i),
                gSession->gammaSelection().numSlices(),
                gSession->gammaSelection().range(),
                &progress));

    interpolate();
}

void TableWidget::interpolate()
{
    TakesLongTime __;

    interpPoints_.clear();
/*
    if (pi) {
        deg alphaStep = pi->stepAlpha.value();
        deg betaStep = pi->stepBeta.value();
        qreal idwRadius = pi->idwRadius.value();

        qreal avgRadius = pi->avgRadius.value();
        qreal avgAlphaMax = pi->avgAlphaMax.value();
        qreal avgTreshold = pi->avgThreshold.value() / 100.0;

        Progress progress(calcPoints_.count(), &progressBar_);

        for_i (calcPoints_.count())
            interpPoints_.append(interpolateInfos(
                calcPoints_.at(i), alphaStep, betaStep, idwRadius, avgAlphaMax, avgRadius,
                avgTreshold, &progress));
    } else {
        for_i (calcPoints_.count())
            interpPoints_.append(PeakInfos());
    }
*/
    displayPeak(getReflIndex());
}

// virtual, overwritten by some output frames, and called back by the overwriting function
void TableWidget::displayPeak(int reflIndex)
{
    dataView_->clear();

//    ASSERT(calcPoints_.count() == interpPoints_.count());
    if (calcPoints_.count() <= reflIndex)
        return;

    bool interpolated = false; // TODO reactivate
    for (const PeakInfo& r : (interpolated ? interpPoints_ : calcPoints_).at(reflIndex))
        dataView_->addRow(r.data(), false);

    dataView_->sortData();
}

int TableWidget::getReflIndex() const
{
    return 1; // TODO URGENT reactivate
}
