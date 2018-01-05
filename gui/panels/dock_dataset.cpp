// ************************************************************************** //
//
//  Steca2: stress and texture calculator
//
//! @file      gui/panels/dock_dataset.cpp
//! @brief     Implements class DockDatasets
//!
//! @homepage  https://github.com/scgmlz/Steca2
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
// ************************************************************************** //

#include "dock_dataset.h"
#include "cfg/gui_cfg.h"
#include "data/suite.h"
#include "models.h"
#include "thehub.h"
#include "widgets/tree_views.h" // inheriting from
#include "widgets/widget_makers.h"




// ************************************************************************** //
//  file-scope: class DatasetView
// ************************************************************************** //

class DatasetView : public ListView {
public:
    DatasetView();

protected:
    void currentChanged(QModelIndex const&, QModelIndex const&);

    DatasetsModel* model() const { return static_cast<DatasetsModel*>(ListView::model()); }
};

DatasetView::DatasetView() : ListView() {
    setModel(gHub->suiteModel); // TODO simplify this
    debug::ensure(dynamic_cast<DatasetsModel*>(ListView::model()));

    connect(gHub, &TheHub::sigSuitesChanged, [this]() {
            gHub->tellSuiteSelected(QSharedPointer<Suite>()); // first de-select
            selectRow(0);
        });
}

void DatasetView::currentChanged(QModelIndex const& current, QModelIndex const& previous) {
    ListView::currentChanged(current, previous);
    gHub->tellSuiteSelected(
        model()->data(current,
                      DatasetsModel::GetDatasetRole).value<QSharedPointer<Suite>>());
}

// ************************************************************************** //
//  class DockDatasets
// ************************************************************************** //

DockDatasets::DockDatasets()
    : DockWidget("Datasets", "dock-suite", Qt::Vertical) {
    box_->addWidget((dataseqView_ = new DatasetView()));

    auto h = hbox();
    box_->addLayout(h);

    h->addWidget(label("Combine:"));
    h->addWidget(combineDatasets_ = spinCell(gui_cfg::em4, 1));
    combineDatasets_->setToolTip("Combine and average number of suite");

    connect(combineDatasets_, slot(QSpinBox, valueChanged, int), [this](int num) {
        gHub->combineDatasetsBy(pint(qMax(1, num)));
    });

    connect(gHub, &TheHub::sigSuitesChanged,
            [this]() { combineDatasets_->setValue(to_i(uint(gHub->suiteGroupedBy()))); });
}



