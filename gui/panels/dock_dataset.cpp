// ************************************************************************** //
//
//  Steca2: stress and texture calculator
//
//! @file      gui/panels/dock_dataset.cpp
//! @brief     Implements class DockDatasets
//!
//! @homepage  https://github.com/scgmlz/Steca2
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2017
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
// ************************************************************************** //

#include "data/datasequence.h"
#include "dock_dataset.h"
#include "widgets/tree_views.h" // inheriting from
#include "gui_cfg.h"
#include "thehub.h"
#include "widgets/widget_makers.h"

namespace gui {
namespace panel {

// ************************************************************************** //
//  file-scope: class DatasetView
// ************************************************************************** //

class DatasetView : public ListView {
public:
    DatasetView();

protected:
    void currentChanged(QModelIndex const&, QModelIndex const&);

    using Model = models::DatasetsModel;
    Model* model() const { return static_cast<Model*>(ListView::model()); }
};

DatasetView::DatasetView() : ListView() {
    setModel(gHub->datasequenceModel); // TODO simplify this
    debug::ensure(dynamic_cast<Model*>(ListView::model()));

    connect(gHub, &TheHubSignallingBase::sigDatasetsChanged, [this]() {
            gHub->tellDatasetSelected(QSharedPointer<DataSequence>()); // first de-select
            selectRow(0);
        });
}

void DatasetView::currentChanged(QModelIndex const& current, QModelIndex const& previous) {
    ListView::currentChanged(current, previous);
    gHub->tellDatasetSelected(
        model()->data(current, Model::GetDatasetRole).value<QSharedPointer<DataSequence>>());
}

// ************************************************************************** //
//  class DockDatasets
// ************************************************************************** //

DockDatasets::DockDatasets()
    : DockWidget("Datasets", "dock-datasequence", Qt::Vertical) {
    box_->addWidget((datasetView_ = new DatasetView()));

    auto h = hbox();
    box_->addLayout(h);

    h->addWidget(label("Combine:"));
    h->addWidget(combineDatasets_ = spinCell(gui_cfg::em4, 1));
    combineDatasets_->setToolTip("Combine and average number of datasequence");

    connect(combineDatasets_, slot(QSpinBox, valueChanged, int), [this](int num) {
        gHub->combineDatasetsBy(pint(qMax(1, num)));
    });

    connect(gHub, &TheHubSignallingBase::sigDatasetsChanged,
            [this]() { combineDatasets_->setValue(to_i(uint(gHub->datasequenceGroupedBy()))); });
}

} // namespace panel
} // namespace gui
