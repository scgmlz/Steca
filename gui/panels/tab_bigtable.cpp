//  ***********************************************************************************************
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
//  ***********************************************************************************************

#include "gui/panels/tab_bigtable.h"
#include "core/session.h"
#include "gui/actions/triggers.h"
#include "gui/mainwin.h"
#include "gui/view/bigtable.h"
//#include "qcr/base/debug.h"

//  ***********************************************************************************************
//! @class ColumnSelector (local scope)

//! A row of controls for choosing which data columns are to be displayed in a TabTable.

class ColumnSelector : public QcrWidget {
public:
    ColumnSelector();
private:
    QcrRadioButton rbAll_ {"rbAll", "all"};
    QcrRadioButton rbNone_ {"rbNone", "none"};
    QcrRadioButton rbInten_ {"rbInten", "Intensity"};
    QcrRadioButton rbTth_ {"rbTth", "2θ"};
    QcrRadioButton rbFWHM_ {"rbFWHM", "fwhm"};
    std::vector<QcrCheckBox*> showCols_;
    void setAll(bool on);
    void updateRadiobuttons();
    using eReflAttr = PeakInfo::eReflAttr;
};

ColumnSelector::ColumnSelector()
{
    auto* box = new QVBoxLayout;
    box->addWidget(&rbAll_  );
    box->addWidget(&rbNone_ );
    box->addWidget(&rbInten_);
    box->addWidget(&rbTth_  );
    box->addWidget(&rbFWHM_ );
    box->addSpacing(8);

    const QStringList& headers = PeakInfo::dataTags(false);
    for (int i=0; i<headers.count(); ++i)
        gSession->params.bigMetaSelection.vec.push_back({true});
    showCols_.resize(headers.count());
    for (int i=0; i<showCols_.size(); ++i) {
        showCols_[i] = new QcrCheckBox(
            "cb"+QString::number(i), headers[i], &gSession->params.bigMetaSelection.vec[i]);
        box->addWidget(showCols_[i]);
    }
    setLayout(box);

    connect(&rbAll_, &QRadioButton::clicked, [this]() {
            setAll(true); });
    connect(&rbNone_, &QRadioButton::clicked, [this]() {
            setAll(false); });
    connect(&rbInten_, &QRadioButton::clicked, [this]() {
            setAll(false);
            showCols_.at(int(eReflAttr::INTEN))->programaticallySetValue(true); });
    connect(&rbTth_, &QRadioButton::clicked, [this]() {
            setAll(false);
            showCols_.at(int(eReflAttr::TTH))->programaticallySetValue(true); });
    connect(&rbFWHM_, &QRadioButton::clicked, [this]() {
            setAll(false);
            showCols_.at(int(eReflAttr::FWHM))->programaticallySetValue(true); });

    setRemake([=](){ updateRadiobuttons(); });
}

void ColumnSelector::setAll(bool on)
{
    for (auto* col : showCols_)
        col->programaticallySetValue(on);
}

void ColumnSelector::updateRadiobuttons()
{
    bool isAll = true, isNone = true, isOther = false;
    int nInten = 0, nTth = 0, nFwhm = 0;

    for (int i=0; i<showCols_.size(); ++i) {
        if (!showCols_.at(i)->getValue()) {
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

    rbNone_.programaticallySetValue(isNone);
    rbAll_.programaticallySetValue(isAll);

    int const PRESET_SELECTION = 1;
    rbInten_.programaticallySetValue(!isOther && PRESET_SELECTION == nInten);
    rbTth_.programaticallySetValue(!isOther && PRESET_SELECTION == nTth);
    rbFWHM_.programaticallySetValue(!isOther && PRESET_SELECTION == nFwhm);
};


//  ***********************************************************************************************
//! @class BigtableTab

BigtableTab::BigtableTab()
{
    auto bigtableView = new BigtableView;

    auto* colSelBox = new QcrScrollArea;
    colSelBox->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    colSelBox->setWidget(new ColumnSelector());

    auto* buttonBox = new QHBoxLayout;
    buttonBox->addStretch(1);
    buttonBox->addWidget(new QcrIconTriggerButton {&gGui->triggers->spawnTable});
    buttonBox->addWidget(new QcrIconTriggerButton {&gGui->triggers->exportBigtable});

    auto* sideBox = new QVBoxLayout;
    sideBox->addWidget(colSelBox);
    sideBox->addLayout(buttonBox);
    sideBox->setStretch(0,1000);

    auto* layout = new QHBoxLayout;
    layout->addWidget(bigtableView);
    layout->addLayout(sideBox);
    layout->setStretch(0,1000);
    setLayout(layout);

    setRemake([=](){bigtableView->refresh();});
}
