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
#include "gui/panels/data_table.h"
#include <QScrollArea>
#include <QThread> // for sleep for debugging
#include <iostream>

// ************************************************************************** //
//  local class ColumnSelector
// ************************************************************************** //

//! A row of controls for choosing which data columns are to be displayed in a TabTable.

class ColumnSelector : public QWidget {
public:
    ColumnSelector(DataView&, const QStringList&);
private:
    DataView& dataView_;
    QVector<CCheckBox*> showCols_;
    CRadioButton rbHidden_ {"rbHidden", ""};
    CRadioButton rbAll_ {"rbAll", "all"};
    CRadioButton rbNone_ {"rbNone", "none"};
    CRadioButton rbInten_ {"rbInten", "Intensity"};
    CRadioButton rbTth_ {"rbTth", "2θ"};
    CRadioButton rbFWHM_ {"rbFWHM", "fwhm"};
    void updateRadiobuttons();
    using eReflAttr = PeakInfo::eReflAttr;
};

ColumnSelector::ColumnSelector(DataView& dataView, const QStringList& headers)
    : dataView_ {dataView}
    , showCols_ {headers.count()}
{
    rbHidden_.hide();

    auto* box = new QVBoxLayout;
    box->addWidget(&rbHidden_);
    box->addWidget(&rbAll_);
    box->addWidget(&rbNone_);
    box->addWidget(&rbInten_);
    box->addWidget(&rbTth_);
    box->addWidget(&rbFWHM_);
    box->addSpacing(8);
    for_i (showCols_.count()) {
        showCols_[i] = new CCheckBox("cb"+QString::number(i), headers[i]);
        box->addWidget(showCols_[i]);
    }
    setLayout(box);

    auto _all = [this]() {
        for (auto* col : showCols_)
            col->setChecked(true); };

    auto _none = [this]() {
        for (auto* col : showCols_)
            col->setChecked(false); };

    auto _showInten = [this, _none]() {
        _none();
        showCols_.at(int(eReflAttr::INTEN))->setChecked(true); };

    auto _showTth = [this, _none]() {
        _none();
        showCols_.at(int(eReflAttr::TTH))->setChecked(true); };

    auto _showFWHM = [this, _none]() {
        _none();
        showCols_.at(int(eReflAttr::FWHM))->setChecked(true); };

    for_i (showCols_.count()) {
        QCheckBox* cb = showCols_.at(i);
        connect(cb, &QCheckBox::toggled, [this, i](bool on) {
                // here we act directly upon dataView_
                if (on)
                    dataView_.showColumn(i + 1);
                else
                    dataView_.hideColumn(i + 1);
                updateRadiobuttons();
            });
    }

    connect(&rbAll_, &QRadioButton::clicked, _all);
    connect(&rbNone_, &QRadioButton::clicked, _none);
    connect(&rbInten_, &QRadioButton::clicked, _showInten);
    connect(&rbTth_, &QRadioButton::clicked, _showTth);
    connect(&rbFWHM_, &QRadioButton::clicked, _showFWHM);

    rbAll_.click();
}

void ColumnSelector::updateRadiobuttons()
{
    bool isAll = true, isNone = true, isOther = false;
    int nInten = 0, nTth = 0, nFwhm = 0;

    for_i (showCols_.count()) {
        if (!showCols_.at(i)->isChecked()) {
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


// ************************************************************************** //
//  class TableWidget
// ************************************************************************** //

TableWidget::TableWidget()
{
    const QStringList& headers = PeakInfo::dataTags(false);
    const QStringList& outHeaders = PeakInfo::dataTags(true);
    const cmp_vec& cmps = PeakInfo::dataCmps();

    dataView_ = new DataView(headers.count()); // the main table
    dataView_->setColumns(headers, outHeaders, cmps);

    // inbound connection
    connect(gSession, &Session::sigRawFits, [this]() {
            if (isVisible())
                render(); });

    // layout
    auto* colSelBox = new QScrollArea;
    colSelBox->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    colSelBox->setWidget(new ColumnSelector(*dataView_, headers));

    auto* buttonBox = new QHBoxLayout;
    buttonBox->addStretch(1);
    buttonBox->addWidget(new XIconButton {&gGui->triggers->spawnTable});
    buttonBox->addWidget(new XIconButton {&gGui->triggers->exportTable});

    auto* sideBox = new QVBoxLayout;
    sideBox->addWidget(colSelBox);
    sideBox->addLayout(buttonBox);
    sideBox->setStretch(0,1000);

    auto* layout = new QHBoxLayout;
    layout->addWidget(dataView_);
    layout->addLayout(sideBox);
    layout->setStretch(0,1000);
    setLayout(layout);
}

void TableWidget::render()
{
    PeakInfos points_ = gSession->peakInfos();
    dataView_->clear();
    for (const PeakInfo& r : points_)
        dataView_->addRow(r.data(), false);
    dataView_->sortData();
}
