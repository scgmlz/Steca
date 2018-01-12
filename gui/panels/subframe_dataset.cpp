// ************************************************************************** //
//
//  Steca: stress and texture calculator
//
//! @file      gui/panels/subframe_dataset.cpp
//! @brief     Implements class SubframeMeasurements
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
// ************************************************************************** //

#include "gui/panels/subframe_dataset.h"
#include "core/data/suite.h"
#include "gui/models.h"
#include "gui/thehub.h"
#include "gui/widgets/tree_views.h" // inheriting from

// ************************************************************************** //
//  file-scope: class MeasurementsView
// ************************************************************************** //

class MeasurementsView : public ListView {
public:
    MeasurementsView();

private:
    void currentChanged(QModelIndex const&, QModelIndex const&);

    MeasurementsModel* model() const { return static_cast<MeasurementsModel*>(ListView::model()); }
};

MeasurementsView::MeasurementsView() : ListView() {
    setModel(gHub->suiteModel); // TODO simplify this
    debug::ensure(dynamic_cast<MeasurementsModel*>(ListView::model()));

    connect(gHub, &TheHub::sigSuitesChanged, [this]() {
            gHub->tellSuiteSelected(shp_Suite()); // first de-select
            selectRow(0);
        });
}

void MeasurementsView::currentChanged(QModelIndex const& current, QModelIndex const& previous) {
    ListView::currentChanged(current, previous);
    gHub->tellSuiteSelected(
        model()->data(current,
                      MeasurementsModel::GetMeasurementRole).value<shp_Suite>());
}

// ************************************************************************** //
//  class SubframeMeasurements
// ************************************************************************** //

SubframeMeasurements::SubframeMeasurements() : DockWidget("Measurements", "dock-suite") {
    box_->addWidget((dataseqView_ = new MeasurementsView()));

    auto h = newQ::HBoxLayout();
    box_->addLayout(h);

    h->addWidget(newQ::Label("Combine:"));
    h->addWidget(combineDatasets_ = newQ::SpinBox(4, false, 1));
    combineDatasets_->setToolTip("Combine and average number of suite");

    connect(combineDatasets_, slot(QSpinBox, valueChanged, int), [this](int num) {
        gHub->combineDatasetsBy(pint(qMax(1, num)));
    });

    connect(gHub, &TheHub::sigSuitesChanged,
            [this]() { combineDatasets_->setValue(to_i(uint(gHub->suiteGroupedBy()))); });
}
