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
#include <QHeaderView>

// ************************************************************************** //
//  local class ExperimentModel
// ************************************************************************** //

//! The model for ExperimentView.

class ExperimentModel final : public TableModel {
public:
    ExperimentModel();

    void showMetaInfo(vec<bool> const&);

    int rowCount() const final { return experiment_.count(); }
    QVariant data(const QModelIndex&, int) const final;
    QVariant headerData(int, Qt::Orientation, int) const final;

    int metaCount() const { return metaInfoNums_.count(); }
    vec<bool> const& rowsChecked() const { return rowsChecked_; }

    enum { COL_CHECK=1, COL_NUMBER, COL_ATTRS };

private:
    int columnCount() const final { return COL_ATTRS + metaCount(); }

    Experiment const& experiment_;
    vec<int> metaInfoNums_; //!< indices of metadata items selected for display
    mutable vec<bool> rowsChecked_;
};

ExperimentModel::ExperimentModel()
    : experiment_(gSession->experiment())
{}

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
    while (rowsChecked_.count()<rowCount())
        rowsChecked_.append(false);
    const Cluster* cluster = gSession->experiment().at(row).data();
    int col = index.column();
    switch (role) {
    case Qt::DisplayRole: {
        if (col==COL_NUMBER) {
            QString ret = QString::number(cluster->first()->totalPosition()+1);
            if (cluster->count()>1)
                ret += "-" + QString::number(cluster->last()->totalPosition()+1);
            return ret;
        } else if (col>=COL_ATTRS && col < COL_ATTRS+metaCount()) {
            return cluster->avgeMetadata()->attributeStrValue(
                metaInfoNums_.at(col-COL_ATTRS));
        } else
            return {};
    }
    case Qt::UserRole:
        return QVariant::fromValue<shp_Cluster>(experiment_.at(row));
    case Qt::ToolTipRole: {
        QString ret;
        if (cluster->count()>1 && cluster->first()->file()==cluster->last()->file()) {
            ret = QString("Measurements %1..%2 are numbers %3..%4 in file %5")
                .arg(cluster->first()->totalPosition()+1)
                .arg(cluster->last()->totalPosition()+1)
                .arg(cluster->first()->position()+1)
                .arg(cluster->last()->position()+1)
                .arg(cluster->last()->file()->fileName());
        } else {
            ret = QString("Measurement %1 is number %2 in file %3")
                .arg(cluster->first()->totalPosition()+1)
                .arg(cluster->first()->position()+1)
                .arg(cluster->first()->file()->fileName());
            if (cluster->count()>1) {
                ret += cluster->count()>2 ? ",...," : ",";
                ret += QString("\nmeasurement %1 is number %2 in file %3")
                    .arg(cluster->last()->totalPosition()+1)
                    .arg(cluster->last()->position()+1)
                    .arg(cluster->last()->file()->fileName());
            }
        }
        ret += ".";
        if (cluster->count()<gSession->experiment().combineBy())
            ret += QString("\nThis cluster has only %1 elements, while the combine factor is %2.")
                .arg(cluster->count())
                .arg(gSession->experiment().combineBy());
        return ret;
    }
    case Qt::TextColorRole: {
        if (cluster->count()>1 &&
            (cluster->first()->file()!=cluster->last()->file()
             || cluster->count()<gSession->experiment().combineBy()))
            return QColor(Qt::red);
        return QColor(Qt::black);
    }
    case Qt::CheckStateRole: {
        if (col==COL_CHECK) {
            return rowsChecked_.at(row) ? Qt::Checked : Qt::Unchecked;
        } else
            return {};
    }
    default:
        return {};
    }
}

QVariant ExperimentModel::headerData(int col, Qt::Orientation ori, int role) const {
    if (ori!=Qt::Horizontal)
        return {};
    if (role != Qt::DisplayRole)
        return {};
    if (col==COL_NUMBER)
        return "#";
    else if (col>=COL_ATTRS && col < COL_ATTRS+metaCount())
        return Metadata::attributeTag(metaInfoNums_.at(col-COL_ATTRS), false);
    return {};
}


// ************************************************************************** //
//  local class ExperimentView
// ************************************************************************** //

//! Main item in SubframeMeasurement: View and control of measurements list.

class ExperimentView final : public ListView {
public:
    ExperimentView();

private:
    void currentChanged(QModelIndex const&, QModelIndex const&) final;
    int sizeHintForColumn(int) const final;
    ExperimentModel* model() const final {
        return static_cast<ExperimentModel*>(ListView::model()); }
};

ExperimentView::ExperimentView() : ListView() {
//    setHeaderHidden(true);
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
    connect(gHub, &TheHub::sigMetatagsChosen, experimentModel,
            [this](vec<bool> const& rowsChecked) {
                model()->showMetaInfo(rowsChecked);
//                setHeaderHidden(model()->metaCount()==0);
            });
}

void ExperimentView::currentChanged(QModelIndex const& current, QModelIndex const& previous) {
    ListView::currentChanged(current, previous);
    gHub->tellClusterSelected(model()->data(current, Qt::UserRole).value<shp_Cluster>());
}

int ExperimentView::sizeHintForColumn(int col) const {
    switch (col) {
    case ExperimentModel::COL_CHECK: {
        return fontMetrics().width('m');
    } default:
        return ListView::sizeHintForColumn(col);
    }
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
