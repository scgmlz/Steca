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
#include "gui/base/tree_views.h" // inheriting from
#include "gui/thehub.h"
#include <QHeaderView>

// ************************************************************************** //
//  local class ExperimentModel
// ************************************************************************** //

//! The model for ExperimentView.

class ExperimentModel final : public TableModel { // < QAbstractTableModel < QAbstractItemModel
public:
    void onClicked(const QModelIndex &);
    void setHighlight(int row);
    void updateMeta(vec<bool> const&);
    void onClustersChanged();
    void onHighlight();

    int rowCount() const final { return allClusters_.count(); }
    QVariant data(const QModelIndex&, int) const final;
    QVariant headerData(int, Qt::Orientation, int) const final;
    int rowHighlighted() const { return rowHighlighted_; }
    int metaCount() const { return metaInfoNums_.count(); }
    vec<bool> const& rowsChecked() const { return rowsChecked_; }

    enum { COL_CHECK=1, COL_NUMBER, COL_ATTRS };

private:
    int columnCount() const final { return COL_ATTRS + metaCount(); }

    vec<int> metaInfoNums_; //!< indices of metadata items selected for display
    vec<bool> rowsChecked_;
    int rowHighlighted_;
    const Cluster* highlighted_{nullptr};
    const QVector<shp_Cluster>& allClusters_ {gSession->dataset().allClusters()};
};

void ExperimentModel::onClicked(const QModelIndex& cell) {
    int row = cell.row();
    if (row < 0 || row >= rowCount())
        return;
    int col = cell.column();
    if (col==1) {
        rowsChecked_[row] = !rowsChecked_[row];
        emit dataChanged(cell, cell);
//        gHub->onClustersActivated(checkedRows());
    } else if (col==2) {
        setHighlight(row);
    }
}

void ExperimentModel::setHighlight(int row) {
    const Cluster* oldHighlighted = highlighted_;
    int oldRow = rowHighlighted_;
    rowHighlighted_ = row;
    if (row>=0) {
        highlighted_ = allClusters_.at(rowHighlighted_).data();
        emit dataChanged(createIndex(row,0),createIndex(row,columnCount()));
    } else
        highlighted_ = nullptr;
    if (oldHighlighted)
        emit dataChanged(createIndex(oldRow,0),createIndex(oldRow,columnCount()));
    if (highlighted_!=oldHighlighted)
        gSession->dataset().setHighlight(highlighted_);
}

void ExperimentModel::updateMeta(vec<bool> const& metadataRows) {
    beginResetModel();
    metaInfoNums_.clear();
    for_i (metadataRows.count())
        if (metadataRows.at(i))
            metaInfoNums_.append(i);
    endResetModel();
}

void ExperimentModel::onClustersChanged() {
    beginResetModel();
    // resize rowsChecked_ according to current Session data
    if (rowsChecked_.count()>rowCount())
        rowsChecked_.resize(rowCount());
    else
        while (rowsChecked_.count()<rowCount())
            rowsChecked_.append(true);

    setHighlight(qMin(rowCount()-1, rowHighlighted_));
    endResetModel();
}

void ExperimentModel::onHighlight() {
    const Cluster& newCluster = gSession->dataset().highlightedCluster();
    if (&newCluster==highlighted_)
        return;
    for (int row=0; row<rowCount(); ++row) {
        if (allClusters_.at(row).data()==&newCluster) {
            setHighlight(row);
            return;
        }
    }
    NEVER
}

QVariant ExperimentModel::data(const QModelIndex& index, int role) const {
    int row = index.row();
    if (row < 0 || row >= rowCount())
        return {};
    const Cluster* cluster = allClusters_.at(row).data();
    int col = index.column();
    switch (role) {
    case Qt::DisplayRole: {
        if (col==COL_NUMBER) {
            QString ret = QString::number(cluster->totalOffset()+1);
            if (cluster->count()>1)
                ret += "-" + QString::number(cluster->totalOffset()+cluster->count());
            return ret;
        } else if (col>=COL_ATTRS && col < COL_ATTRS+metaCount()) {
            return cluster->avgeMetadata()->attributeStrValue(
                metaInfoNums_.at(col-COL_ATTRS));
        } else
            return {};
    }
    case Qt::ToolTipRole: {
        QString ret;
        if (cluster->count()>1) {
            ret = QString("Measurements %1..%2 are numbers %3..%4 in file %5")
                .arg(cluster->totalOffset()+1)
                .arg(cluster->totalOffset()+cluster->count())
                .arg(cluster->offset()+1)
                .arg(cluster->offset()+cluster->count())
                .arg(cluster->file().name());
        } else {
            ret = QString("Measurement %1 is number %2 in file %3")
                .arg(cluster->totalOffset()+1)
                .arg(cluster->offset()+1)
                .arg(cluster->file().name());
        }
        ret += ".";
        if (cluster->isIncomplete())
            ret += QString("\nThis cluster has only %1 elements, while the combine factor is %2.")
                .arg(cluster->count())
                .arg(gSession->dataset().binning());
        return ret;
    }
    case Qt::ForegroundRole: {
        if (col==COL_NUMBER && cluster->count()>1 &&
            (cluster->isIncomplete()))
            return QColor(Qt::red);
        return QColor(Qt::black);
    }
    case Qt::BackgroundRole: {
        if (row==rowHighlighted_)
            return QColor(Qt::cyan);
        return QColor(Qt::white);
    }
    case Qt::CheckStateRole: {
        if (col==COL_CHECK)
            return rowsChecked_.at(row) ? Qt::Checked : Qt::Unchecked;
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

class ExperimentView final : public ListView { // < QTreeView < QAbstractItemView
public:
    ExperimentView();

private:
    void currentChanged(QModelIndex const&, QModelIndex const&) override final;
    void onClustersChanged();
    void onHighlight();
    void updateScroll();
    int sizeHintForColumn(int) const override final;
    ExperimentModel* model() const final {
        return static_cast<ExperimentModel*>(ListView::model()); }
};

ExperimentView::ExperimentView() : ListView() {
    setHeaderHidden(true);
    setSelectionMode(QAbstractItemView::NoSelection);
    auto experimentModel = new ExperimentModel();
    setModel(experimentModel);
    /* TODO replace
    connect(gHub, &TheHub::sigFilesSelected, model(), &ExperimentModel::onFilesChanged);
    connect(gHub, &TheHub::sigClustersChanged,
            [this]() {
                model()->signalReset();
                setCurrentIndex(model()->index(0,0));
            });
    connect(gHub, &TheHub::sigMetatagsChosen, experimentModel,
            [this](vec<bool> const& rowsChecked) {
                model()->updateMeta(rowsChecked);
                setHeaderHidden(model()->metaCount()==0);
            });
    */
    connect(gSession, &Session::sigClusters, this, &ExperimentView::onClustersChanged);
    connect(gSession, &Session::sigHighlight, this, &ExperimentView::onHighlight);
    connect(this, &ExperimentView::clicked, model(), &ExperimentModel::onClicked);
}

//! Overrides QAbstractItemView. This slot is called when a new item becomes the current item.
void ExperimentView::currentChanged(QModelIndex const& current, QModelIndex const& previous) {
    model()->setHighlight(current.row());
    updateScroll();
}

void ExperimentView::onClustersChanged() {
    model()->onClustersChanged();
    updateScroll();
}

void ExperimentView::onHighlight() {
    model()->onHighlight();
    updateScroll();
}

void ExperimentView::updateScroll() {
    scrollTo(model()->index(model()->rowHighlighted(),0));
}

int ExperimentView::sizeHintForColumn(int col) const {
    switch (col) {
    case ExperimentModel::COL_CHECK: {
        return 2*mWidth();
    } default:
        return 3*mWidth();
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
    controls_row->addWidget(newQ::Label("combine:"));
    auto combineMeasurements = newQ::SpinBox(4, false, 1);
    controls_row->addWidget(combineMeasurements);
    combineMeasurements->setToolTip("Combine and average number of cluster");
    connect(combineMeasurements, _SLOT_(QSpinBox, valueChanged, int),
            [this](int num) { gSession->dataset().setBinning(num); });
    // TODO restore back connection
    //connect(gHub, &TheHub::sigClustersChanged,
    //        [=]() { combineMeasurements->setValue(gSession->dataset().binning()); });

    // 'if incomplete' control
    controls_row->addStretch(1);
    auto ifIncompleteLabel = newQ::Label("if incomplete:");
    controls_row->addWidget(ifIncompleteLabel);
    auto ifIncomplete = new QComboBox;
    ifIncomplete->addItems({"keep", "drop"});
    controls_row->addWidget(ifIncomplete);
    connect(ifIncomplete, _SLOT_(QComboBox, currentIndexChanged, int),
            [this](int index) { gSession->dataset().setDropIncomplete(index); });
    connect(gSession, &Session::sigClusters, [=]() {
            if (gSession->dataset().hasIncomplete()) {
                ifIncompleteLabel->show(); ifIncomplete->show();
            } else {
                ifIncompleteLabel->hide(); ifIncomplete->hide();
            } });

    // TODO: add back connection (json->display)
    // TODO: deactivate this control if there is nothing to drop
}
