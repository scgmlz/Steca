// ************************************************************************** //
//
//  Steca: stress and texture calculator
//
//! @file      gui/panels/subframe_dataset.cpp
//! @brief     Implements class SubframeDatasets
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
// ************************************************************************** //

#include "gui/panels/subframe_dataset.h"
#include "core/data/suite.h"
#include "gui/cfg/gui_cfg.h"
#include "gui/models.h"
#include "gui/thehub.h"
#include "gui/widgets/tree_views.h" // inheriting from

// ************************************************************************** //
//  file-scope: class DatasetView
// ************************************************************************** //

class DatasetView : public ListView {
public:
    DatasetView();

private:
    void currentChanged(QModelIndex const&, QModelIndex const&);

    DatasetsModel* model() const { return static_cast<DatasetsModel*>(ListView::model()); }
};

DatasetView::DatasetView() : ListView() {
    setModel(gHub->suiteModel); // TODO simplify this
    debug::ensure(dynamic_cast<DatasetsModel*>(ListView::model()));

    connect(gHub, &TheHub::sigSuitesChanged, [this]() {
            gHub->tellSuiteSelected(shp_Suite()); // first de-select
            selectRow(0);
        });
}

void DatasetView::currentChanged(QModelIndex const& current, QModelIndex const& previous) {
    ListView::currentChanged(current, previous);
    gHub->tellSuiteSelected(
        model()->data(current,
                      DatasetsModel::GetDatasetRole).value<shp_Suite>());
}

// ************************************************************************** //
//  class SubframeDatasets
// ************************************************************************** //

SubframeDatasets::SubframeDatasets() : DockWidget("Datasets", "dock-suite") {
    box_->addWidget((dataseqView_ = new DatasetView()));

    auto h = newQ::HBoxLayout();
    box_->addLayout(h);

    h->addWidget(newQ::Label("Combine:"));
    h->addWidget(combineDatasets_ = newQ::SpinBox(gui_cfg::em4, 1));
    combineDatasets_->setToolTip("Combine and average number of suite");

    connect(combineDatasets_, slot(QSpinBox, valueChanged, int), [this](int num) {
        gHub->combineDatasetsBy(pint(qMax(1, num)));
    });

    connect(gHub, &TheHub::sigSuitesChanged,
            [this]() { combineDatasets_->setValue(to_i(uint(gHub->suiteGroupedBy()))); });
}
