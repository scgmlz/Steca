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
    ColumnSelectorModel() : CheckTableModel("colSel") {}

private:
    int highlighted() const final { return highlighted_; }
    void onHighlight(int i) final { highlighted_ = i; }
    bool activated(int row) const { return sel_->isSelected(row); }
    void setActivated(int row, bool on) { sel_->set(row, on); }

    int columnCount() const final { return 3; }
    int rowCount() const final { return sel_->size(); }

    QVariant data(const QModelIndex&, int) const;
    QVariant headerData(int, Qt::Orientation, int) const { return {}; }

    int highlighted_ {0};
    BoolMap*const sel_ { &gSession->params.bigMetaSelection };
};

QVariant ColumnSelectorModel::data(const QModelIndex& index, int role) const
{
    int row = index.row();
    int col = index.column();
    if (row < 0 || rowCount() <= row)
        return {};
    switch (role) {
    case Qt::CheckStateRole:
        if (col==1)
            return sel_->isSelected(row) ? Qt::Checked : Qt::Unchecked;
        break;
    case Qt::DisplayRole:
        if (col==2)
            return sel_->keyAt(row);
        return "";
    case Qt::BackgroundRole:
        return QColor(Qt::white);
    }
    return {};
}

//  ***********************************************************************************************
//! @class ColumnSelectorView (local scope)

//! Main item in SubframeMetadata: View and control the list of Metadata.

class ColumnSelectorView : public CheckTableView {
public:
    ColumnSelectorView();
private:
    ColumnSelectorModel* model() { return static_cast<ColumnSelectorModel*>(model_); }
};

ColumnSelectorView::ColumnSelectorView()
    : CheckTableView(new ColumnSelectorModel())
{
    setColumnWidth(0, 0);
    setColumnWidth(1,  .5*mWidth());
    setColumnWidth(2, 6. *mWidth());
    setColumnWidth(3, 7.5*mWidth());
    setRemake([this](){
                  gSession->params.bigMetaSelection.replaceKeys(PeakInfo::dataTags(false), false);
                  onData();
              });
}

//  ***********************************************************************************************
//! @class BigtableTab

BigtableTab::BigtableTab()
    : QcrWidget("BigtableTab")
{
    auto* bigtableView = new BigtableView;

    auto* trigAll   = new QcrIconTriggerButton
        {"bigtabAll", "select all columns", ":/icon/All"};
    auto* trigClear = new QcrIconTriggerButton
        {"bigtabClear", "unselect all columns", ":/icon/clear"};

    trigAll  ->trigger()->setTriggerHook(
        [this](){ gSession->params.bigMetaSelection.setAll(true);  });
    trigClear->trigger()->setTriggerHook(
        [this](){ gSession->params.bigMetaSelection.setAll(false); });

    auto* buttonBox = new QHBoxLayout;
    buttonBox->addWidget(trigAll);
    buttonBox->addWidget(trigClear);
    buttonBox->addStretch(1);
    buttonBox->addWidget(new QcrIconTriggerButton {&gGui->triggers->spawnTable});
    buttonBox->addWidget(new QcrIconTriggerButton {&gGui->triggers->exportBigtable});

    auto* sideBox = new QVBoxLayout;
    sideBox->addWidget(new ColumnSelectorView);
    sideBox->addLayout(buttonBox);
    sideBox->setStretch(0,1000);

    auto* layout = new QHBoxLayout;
    layout->addWidget(bigtableView);
    layout->addLayout(sideBox);
    layout->setStretch(0,1000);
    setLayout(layout);
    setRemake([=](){ bigtableView->refresh(); });

}
