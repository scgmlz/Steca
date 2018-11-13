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
#include <QButtonGroup>
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
    void setOne(int pos, bool on);
    void setAll(bool on);
    void updateRadiobuttons();
    using eReflAttr = PeakInfo::eReflAttr;
};

ColumnSelector::ColumnSelector()
    : QcrWidget("ColumnSelector")
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

    rbAll_.cell()->setHook([this](bool on) {
        if (on)
            setAll(true);
    });
    rbNone_.cell()->setHook([this](bool on) {
        if (on)
            setAll(false);
    });
    rbInten_.cell()->setHook([this](bool on) {
        if (on) {
            setAll(false);
            setOne(int(eReflAttr::INTEN), true);
        }
    });
    rbTth_.cell()->setHook([this](bool on) {
        if (on) {
            setAll(false);
            setOne(int(eReflAttr::TTH), true);
        }
    });
    rbFWHM_.cell()->setHook([this](bool on) {
        if (on) {
            setAll(false);
            setOne(int(eReflAttr::FWHM), true);
        }
    });

    setRemake([=](){ updateRadiobuttons(); });
}

void ColumnSelector::setOne(int pos, bool on)
{
    gSession->params.bigMetaSelection.vec.at(pos).pureSetVal(on);
}

void ColumnSelector::setAll(bool on)
{
    for (auto& col : gSession->params.bigMetaSelection.vec)
        col.pureSetVal(on);
}

void ColumnSelector::updateRadiobuttons()
{
    //bool isAll = true, isNone = true, isOther = false;
    bool isAll = true;
    bool isNone = true;
    bool isInten = true;
    bool isTth = true;
    bool isFwhm = true;

    for (int i=0; i<gSession->params.bigMetaSelection.vec.size(); ++i) {
        const bool isOn = gSession->params.bigMetaSelection.vec.at(i).val();
        const bool isAlphOrBeta = (eReflAttr(i)==eReflAttr::ALPHA)
            || (eReflAttr(i)==eReflAttr::BETA);

        isAll   &= isOn;
        isNone  &= !isOn;
        isInten &= (isOn == (eReflAttr(i) == eReflAttr::INTEN)) || isAlphOrBeta;
        isTth   &= (isOn == (eReflAttr(i) == eReflAttr::TTH)  ) || isAlphOrBeta;
        isFwhm  &= (isOn == (eReflAttr(i) == eReflAttr::FWHM) ) || isAlphOrBeta;
    }

    rbAll_.cell()->pureSetVal(isAll);
    rbNone_.cell()->pureSetVal(isNone);
    rbInten_.cell()->pureSetVal(isInten);
    rbTth_.cell()->pureSetVal(isTth);
    rbFWHM_.cell()->pureSetVal(isFwhm);
};


//  ***********************************************************************************************
//! @class BigtableTab

BigtableTab::BigtableTab()
    : QcrWidget("BigtableTab")
{
    auto bigtableView = new BigtableView;

    auto* colSelBox = new QcrScrollArea("colSelBox");
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
