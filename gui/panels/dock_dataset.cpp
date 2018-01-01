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

#include "dock_dataset.h"
#include "views.h" // inheriting from
#include "gui_cfg.h"
#include "thehub.h"
#include "widget_makers.h"
#include <QSpinBox>

namespace gui {
namespace panel {

// ************************************************************************** //
//  file-scope: class DatasetView
// ************************************************************************** //

class DatasetView : public views::ListView {
public:
    DatasetView();

protected:
    void currentChanged(QModelIndex const&, QModelIndex const&);

    using Model = models::DatasetsModel;
    Model* model() const { return static_cast<Model*>(views::ListView::model()); }
};

DatasetView::DatasetView() : views::ListView() {
    setModel(&gHub->datasetsModel); // TODO simplify this
    debug::ensure(dynamic_cast<Model*>(views::ListView::model()));

    connect(gHub, &TheHubSignallingBase::sigDatasetsChanged, [this]() {
            gHub->tellDatasetSelected(data::shp_Dataset()); // first de-select
            selectRow(0);
        });
}

void DatasetView::currentChanged(QModelIndex const& current, QModelIndex const& previous) {
    views::ListView::currentChanged(current, previous);
    gHub->tellDatasetSelected(model()->data(current,
                                           Model::GetDatasetRole).value<data::shp_Dataset>());
}

// ************************************************************************** //
//  class DockDatasets
// ************************************************************************** //

DockDatasets::DockDatasets()
    : DockWidget("Datasets", "dock-datasets", Qt::Vertical) {
    box_->addWidget((datasetView_ = new DatasetView()));

    auto h = hbox();
    box_->addLayout(h);

    h->addWidget(label("Combine:"));
    h->addWidget(combineDatasets_ = spinCell(gui_cfg::em4, 1));
    combineDatasets_->setToolTip("Combine and average number of datasets");

    connect(combineDatasets_, slot(QSpinBox, valueChanged, int), [this](int num) {
        gHub->combineDatasetsBy(pint(qMax(1, num)));
    });

    connect(gHub, &TheHubSignallingBase::sigDatasetsChanged,
            [this]() { combineDatasets_->setValue(to_i(uint(gHub->datasetsGroupedBy()))); });
}

} // namespace panel
} // namespace gui
