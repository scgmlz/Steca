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
//! @class ColConButtons (local scope)

class ColConButtons {
public:
    ColConButtons();
    QcrTrigger* trigAll;
    QcrTrigger* trigClear;
private:
    void setOne(int pos, bool on);
    void setAll(bool on);
};

ColConButtons::ColConButtons()
{
    trigAll   = new QcrTrigger {"bigtabAll", "select all columns", ":/icon/All"};
    trigClear = new QcrTrigger {"bigtabClear", "unselect all columns", ":/icon/clear"};

    trigAll  ->setTriggerHook([this](){ setAll(true);  });
    trigClear->setTriggerHook([this](){ setAll(false); });
}

void ColConButtons::setOne(int pos, bool on)
{
    gSession->params.bigMetaSelection.set(pos,on);
}

void ColConButtons::setAll(bool on)
{
    gSession->params.bigMetaSelection.setAll(on);
}

//  ***********************************************************************************************
//! @class ColumnsControl (local scope)

//! A row of controls for choosing which data columns are to be displayed in a TabTable.

//! User actions become effective through the general remake hook,
//! which invokes BigtableView::refresh and BigtableView::updateShownColumns().

class ColumnsControl : public QcrWidget {
public:
    ColumnsControl() = delete;
    ColumnsControl(ColConButtons*);
};

ColumnsControl::ColumnsControl(ColConButtons* colConButtons)
    : QcrWidget("ColumnsControl")
{
    auto* hb = new QHBoxLayout;
    hb->addSpacing(4);
    hb->addStretch(1);
    hb->addWidget(new QcrIconTriggerButton(colConButtons->trigAll));
    hb->addWidget(new QcrIconTriggerButton(colConButtons->trigClear));
    hb->addSpacing(4);

    auto* box = new QVBoxLayout;
    box->addLayout(hb);
    box->addSpacing(8);

    QStringList headers = PeakInfo::dataTags(false);
    gSession->params.bigMetaSelection.replaceKeys(headers);
    for (const QString& name: headers) {
        gSession->params.bigMetaSelection.set(name, true);
        QcrCell<bool>* cell = gSession->params.bigMetaSelection.cellAt(name);
        box->addWidget(new QcrCheckBox("bigtable_"+name, name, cell));
    }
    setLayout(box);
}

//  ***********************************************************************************************
//! @class BigtableTab

BigtableTab::BigtableTab()
    : QcrWidget("BigtableTab")
{
    bigtableView = new BigtableView;
    colConButtons = new ColConButtons;

    colSelBox = new QcrScrollArea("colSelBox");
    colSelBox->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

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

    setRemake([this](){
                  qDebug() << "DEBUG remake BT";
                  if (cc) {
                      qDebug() << "DEBUG going to delete cc";
                      delete cc;
                      qDebug() << "DEBUG survived deletion of cc";
                  } else
                      qDebug() << "DEBUG there was no cc";
                  cc = new ColumnsControl(colConButtons);
                  qDebug() << "DEBUG and created new cc";
                  colSelBox->setWidget(cc);
                  bigtableView->refresh();
              });
}
