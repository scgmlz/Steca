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
    auto* box = new QVBoxLayout;
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

    colSelBox = new QcrScrollArea("colSelBox");
    colSelBox->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    auto* trigAll   = new QcrTrigger {"bigtabAll", "select all columns", ":/icon/All"};
    auto* trigClear = new QcrTrigger {"bigtabClear", "unselect all columns", ":/icon/clear"};

    trigAll  ->setTriggerHook([this](){ gSession->params.bigMetaSelection.setAll(true);  });
    trigClear->setTriggerHook([this](){ gSession->params.bigMetaSelection.setAll(false); });

    auto* buttonBox = new QHBoxLayout;
    buttonBox->addWidget(new QcrIconTriggerButton(trigAll));
    buttonBox->addWidget(new QcrIconTriggerButton(trigClear));
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
                  cc = new ColumnsControl(colConButtons);
                  gRoot->pushDeletable(cc);
                  colSelBox->setWidget(cc);
                  bigtableView->refresh();
              });
}
