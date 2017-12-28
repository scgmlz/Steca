// ************************************************************************** //
//
//  Steca2: stress and texture calculator
//
//! @file      gui/panels/dock_dataset.cpp
//! @brief     Implements ...
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

namespace gui {
namespace panel {

class DatasetView : public views::ListView {
private:
    using super = views::ListView;
public:
    DatasetView(TheHub&);

protected:
    void currentChanged(QModelIndex const&, QModelIndex const&);

    using Model = models::DatasetsModel;
    Model* model() const { return static_cast<Model*>(super::model()); }
};

DatasetView::DatasetView(TheHub& hub) : super(hub) {
    setModel(&hub.datasetsModel);
    debug::ensure(dynamic_cast<Model*>(super::model()));

    connect(&hub_, &TheHubSignallingBase::sigDatasetsChanged, [this]() {
            hub_.tellDatasetSelected(data::shp_Dataset()); // first de-select
            selectRow(0);
        });
}

void DatasetView::currentChanged(QModelIndex const& current, QModelIndex const& previous) {
    super::currentChanged(current, previous);
    hub_.tellDatasetSelected(model()->data(current,
                                           Model::GetDatasetRole).value<data::shp_Dataset>());
}

DockDatasets::DockDatasets(TheHub& hub)
    : super("Datasets", "dock-datasets", Qt::Vertical), hub_(hub) {
    box_->addWidget((datasetView_ = new DatasetView(hub)));

    auto h = hbox();
    box_->addLayout(h);

    h->addWidget(label("Combine:"));
    h->addWidget(combineDatasets_ = spinCell(gui_cfg::em4, 1));
    combineDatasets_->setToolTip("Combine and average number of datasets");

    connect(combineDatasets_, slot(QSpinBox, valueChanged, int), [this](int num) {
        hub_.combineDatasetsBy(pint(qMax(1, num)));
    });

    connect(&hub_, &TheHubSignallingBase::sigDatasetsChanged,
            [this]() { combineDatasets_->setValue(to_i(uint(hub_.datasetsGroupedBy()))); });
}

} // namespace panel
} // namespace gui
