// ************************************************************************** //
//
//  Steca: stress and texture calculator
//
//! @file      gui/panels/subframe_measurements.cpp
//! @brief     Implements class SubframeMeasurements
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
// ************************************************************************** //

#include "gui/panels/subframe_measurements.h"
#include "core/session.h"
#include "gui/base/table_model.h"
#include "gui/thehub.h"
#include "gui/base/tree_views.h" // inheriting from


// ************************************************************************** //
//  local class ExperimentModel
// ************************************************************************** //

//! The model for ExperimentView.

class ExperimentModel : public TableModel {
public:
    ExperimentModel() : experiment_(gSession->experiment()) {}

    void showMetaInfo(vec<bool> const&);

    int columnCount() const final { return COL_ATTRS + metaInfoNums_.count(); }
    int rowCount() const final { return experiment_.count(); }
    QVariant data(const QModelIndex&, int) const;
    QVariant headerData(int, Qt::Orientation, int) const;

    enum { COL_NUMBER = 1, COL_ATTRS };

private:
    Experiment const& experiment_;
    vec<int> metaInfoNums_; //!< indices of metadata items selected for display
};

void ExperimentModel::showMetaInfo(vec<bool> const& metadataRows) {
    beginResetModel();
    metaInfoNums_.clear();
    for_i (metadataRows.count())
        if (metadataRows.at(i))
            metaInfoNums_.append(i);
    endResetModel();
}

QVariant ExperimentModel::data(const QModelIndex& index, int role) const {
    int row = index.row();
    if (row < 0 || row >= rowCount())
        return {};
    switch (role) {
    case Qt::DisplayRole: {
        int col = index.column();
        if (col < 1 || col >= columnCount())
            return {};
        switch (col) {
        case COL_NUMBER:
            return gSession->experiment().at(row)->tag();
        default:
            return experiment_.at(row)->avgeMetadata()->attributeStrValue(
                metaInfoNums_.at(col-COL_ATTRS));
        }
    }
    case Qt::UserRole:
        return QVariant::fromValue<shp_Cluster>(experiment_.at(row));
    default:
        return {};
    }
}

QVariant ExperimentModel::headerData(int col, Qt::Orientation, int role) const {
    if (Qt::DisplayRole != role || col < 1 || columnCount() <= col)
        return {};
    switch (col) {
    case COL_NUMBER:
        return "#";
    default:
        return Metadata::attributeTag(metaInfoNums_.at(col-COL_ATTRS), false);
    }
}


// ************************************************************************** //
//  local class ExperimentView
// ************************************************************************** //

//! Main item in SubframeMeasurement: View and control of measurements list.

class ExperimentView final : public ListView {
public:
    ExperimentView();

private:
    void currentChanged(QModelIndex const&, QModelIndex const&);

    ExperimentModel* model() const override {
        return static_cast<ExperimentModel*>(ListView::model()); }
};

ExperimentView::ExperimentView() : ListView() {
    auto experimentModel = new ExperimentModel();
    setModel(experimentModel);
    connect(gHub, &TheHub::sigClustersChanged,
            [=]() { experimentModel->signalReset(); });
    debug::ensure(dynamic_cast<ExperimentModel*>(ListView::model()));

    connect(gHub, &TheHub::sigClustersChanged,
            [this]() {
                gHub->tellClusterSelected(shp_Cluster()); // first de-select
                selectRow(0);
            });
    connect(gHub, &TheHub::sigMetatagsChosen, experimentModel, &ExperimentModel::showMetaInfo);
}

void ExperimentView::currentChanged(QModelIndex const& current, QModelIndex const& previous) {
    ListView::currentChanged(current, previous);
    gHub->tellClusterSelected(model()->data(current, Qt::UserRole).value<shp_Cluster>());
}

// ************************************************************************** //
//  class SubframeMeasurements
// ************************************************************************** //

SubframeMeasurements::SubframeMeasurements() : DockWidget("Measurements", "dock-cluster") {

    // subframe item #1: list of measurements
    box_->addWidget(new ExperimentView());

    // subframe item #2: controls row
    auto controls_row = newQ::HBoxLayout();
    box_->addLayout(controls_row);

    // 'combine' control
    controls_row->addWidget(newQ::Label("Combine:"));
    auto combineMeasurements = newQ::SpinBox(4, false, 1);
    controls_row->addWidget(combineMeasurements);
    combineMeasurements->setToolTip("Combine and average number of cluster");
    connect(combineMeasurements, _SLOT_(QSpinBox, valueChanged, int),
            [this](int num) { gHub->combineMeasurementsBy(qMax(1, num)); });
    connect(gHub, &TheHub::sigClustersChanged,
            [=]() { combineMeasurements->setValue(gHub->clusterGroupedBy()); });
}
