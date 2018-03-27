// ************************************************************************** //
//
//  Steca: stress and texture calculator
//
//! @file      gui/panels/tab_bigtable.cpp
//! @brief     Implements class BigtableTab, and its dependences
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
// ************************************************************************** //

#include "tab_bigtable.h"
#include "core/session.h"
#include "gui/actions/triggers.h"
#include "gui/view/bigtable.h"
#include "gui/mainwin.h"
#include "gui/state.h"
#include <QScrollArea>
#include <QThread> // for sleep for debugging

// ************************************************************************** //
//  local class ColumnSelector
// ************************************************************************** //

//! A row of controls for choosing which data columns are to be displayed in a TabTable.

class ColumnSelector : public QWidget {
public:
    ColumnSelector();
private:
    QVector<CCheckBox*> showCols_;
    CRadioButton rbHidden_ {"rbHidden", ""};
    CRadioButton rbAll_ {"rbAll", "all"};
    CRadioButton rbNone_ {"rbNone", "none"};
    CRadioButton rbInten_ {"rbInten", "Intensity"};
    CRadioButton rbTth_ {"rbTth", "2θ"};
    CRadioButton rbFWHM_ {"rbFWHM", "fwhm"};
    void setAll(bool on);
    void updateRadiobuttons();
    using eReflAttr = PeakInfo::eReflAttr;
};

ColumnSelector::ColumnSelector()
{
    const QStringList& headers = PeakInfo::dataTags(false);
    rbHidden_.hide();

    auto* box = new QVBoxLayout;
    box->addWidget(&rbHidden_);
    box->addWidget(&rbAll_);
    box->addWidget(&rbNone_);
    box->addWidget(&rbInten_);
    box->addWidget(&rbTth_);
    box->addWidget(&rbFWHM_);
    box->addSpacing(8);
    showCols_.resize(headers.count());
    for_i (showCols_.count()) {
        showCols_[i] = new CCheckBox("cb"+QString::number(i), headers[i]);
        box->addWidget(showCols_[i]);
    }
    setLayout(box);

    connect(&rbAll_, &QRadioButton::clicked, [this]() {
            setAll(true); });
    connect(&rbNone_, &QRadioButton::clicked, [this]() {
            setAll(false); });
    connect(&rbInten_, &QRadioButton::clicked, [this]() {
            setAll(false);
            showCols_.at(int(eReflAttr::INTEN))->setChecked(true); });
    connect(&rbTth_, &QRadioButton::clicked, [this]() {
            setAll(false);
            showCols_.at(int(eReflAttr::TTH))->setChecked(true); });
    connect(&rbFWHM_, &QRadioButton::clicked, [this]() {
            setAll(false);
            showCols_.at(int(eReflAttr::FWHM))->setChecked(true); });

    for_i (showCols_.count()) {
        QCheckBox* cb = showCols_.at(i);
        connect(cb, &QCheckBox::toggled, [this, i](bool on) {
                gGui->state->bigtableShowCol[i] = on;
                updateRadiobuttons();
                emit gSession->sigBigtableCols();
            });
    }

    rbAll_.click();
}

void ColumnSelector::setAll(bool on)
{
    for (auto* col : showCols_)
        col->setChecked(on);
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
//  class BigtableTab
// ************************************************************************** //

BigtableTab::BigtableTab()
    : dataView_ {new DataView()}
{
    // inbound connection
    connect(gSession, &Session::sigRawFits, [this]() { render(); });

    // layout
    auto* colSelBox = new QScrollArea;
    colSelBox->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    colSelBox->setWidget(new ColumnSelector());

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

void BigtableTab::render()
{
    if (!isVisible())
        return;
    dataView_->refresh();
}
