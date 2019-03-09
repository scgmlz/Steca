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
#include "qcr/widgets/tables.h"
#include "core/session.h"
#include "gui/actions/triggers.h"
#include "gui/mainwin.h"
#include "gui/view/bigtable.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QButtonGroup>
//#include "qcr/base/debug.h"

//  ***********************************************************************************************
//! @class ColumnSelectorModel (local scope)

//! The model for MetadatView.

class ColumnSelectorModel : public CheckTableModel {
public:
    ColumnSelectorModel() : CheckTableModel{"colSel"} {}

private:
    int highlighted() const final { return highlighted_; }
    void onHighlight(int i) final { highlighted_ = i; }
    bool activated(int row) const { return sel_->isSelected(row); }
    void setActivated(int row, bool on) { sel_->set(row, on); }

    int columnCount() const final { return 3; }
    int rowCount() const final { return sel_->size(); }

    QVariant entry(int, int) const final;
    QVariant headerData(int, Qt::Orientation, int) const { return {}; }

    int highlighted_ {0};
    BoolMap*const sel_ { &gSession->params.bigMetaSelection };
};

QVariant ColumnSelectorModel::entry(int row, int col) const
{
    if (col==2)
        return sel_->keyAt(row);
    return "";
}

//  ***********************************************************************************************
//! @class ColumnSelectorView (local scope)

//! Main item in SubframeMetadata: View and control the list of Metadata.

class ColumnSelectorView : public CheckTableView {
public:
    ColumnSelectorView();
    void onData() override;
private:
    ColumnSelectorModel* model() { return static_cast<ColumnSelectorModel*>(model_); }
};

ColumnSelectorView::ColumnSelectorView()
    : CheckTableView{new ColumnSelectorModel{}}
{
    setColumnWidth(0, 0);
    setColumnWidth(1,  .5*mWidth());
    setColumnWidth(2, 6. *mWidth());
    setColumnWidth(3, 7.5*mWidth());
    setRemake([](){}); // for the time being, remake is steered by BigtableTab
}

void ColumnSelectorView::onData()
{
    // TODO: replace the following ad-hoc statement by proper lazy evaluation.
    // Then revert to standard remake mechanism for this class and for BigTableView.
    gSession->params.bigMetaSelection.replaceKeys(gSession->allNiceKeys(), false);
    model_->refreshModel();
    emit model_->layoutChanged(); // TODO merge into base class ?
    updateScroll();
}


//  ***********************************************************************************************
//! @class BigtableTab

BigtableTab::BigtableTab()
    : QcrWidget{"BigtableTab"}
{
    auto* bigtableView = new BigtableView;
    auto* columnSelectorView = new ColumnSelectorView;

    auto* trigAll   = new QcrIconTriggerButton
        {"bigtabAll", "select all columns", ":/icon/All"};
    auto* trigClear = new QcrIconTriggerButton
        {"bigtabClear", "unselect all columns", ":/icon/clear"};

    trigAll  ->trigger()->setTriggerHook(
        [](){ gSession->params.bigMetaSelection.setAll(true);  });
    trigClear->trigger()->setTriggerHook(
        [](){ gSession->params.bigMetaSelection.setAll(false); });

    auto* buttonBox = new QHBoxLayout;
    buttonBox->addWidget(trigAll);
    buttonBox->addWidget(trigClear);
    buttonBox->addStretch(1);
    buttonBox->addWidget(new QcrIconTriggerButton {&gGui->triggers->spawnTable});
    buttonBox->addWidget(new QcrIconTriggerButton {&gGui->triggers->exportBigtable});

    auto* sideBox = new QVBoxLayout;
    sideBox->addWidget(columnSelectorView);
    sideBox->addLayout(buttonBox);
    sideBox->setStretch(0,1000);

    auto* layout = new QHBoxLayout;
    layout->addWidget(bigtableView);
    layout->addLayout(sideBox);
    layout->setStretch(0,1000);
    setLayout(layout);
    // does remake order matter ? otherwise we could setRemake in the member classes
    setRemake([=](){ columnSelectorView->onData(); bigtableView->onData(); });
}
