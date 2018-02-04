// ************************************************************************** //
//
//  Steca: stress and texture calculator
//
//! @file      gui/panels/subframe_measurements.cpp
//! @brief     Implements class SubframeMeasurements, with local model and view
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
// ************************************************************************** //

#include "gui/panels/subframe_measurements.h"
#include "core/session.h"
#include "gui/base/model_view.h"
#include "gui/thehub.h"
#include <QHeaderView>

// ************************************************************************** //
//  local class ExperimentModel
// ************************************************************************** //

//! The model for ExperimentView.

class ExperimentModel : public TableModel { // < QAbstractTableModel < QAbstractItemModel
public:
    void onClicked(const QModelIndex &);
    void onClustersChanged();
    void onHighlight();
    void onActivated();
    void onMetaSelection();

    int metaCount() const { return metaInfoNums_.count(); }

    enum { COL_CHECK=1, COL_NUMBER, COL_ATTRS };

private:
    QVariant data(const QModelIndex&, int) const final;
    QVariant headerData(int, Qt::Orientation, int) const final;
    int rowCount() const final { return gSession->dataset().countClusters(); }
    int columnCount() const final { return COL_ATTRS + metaCount(); }

    // The following local caches duplicate state information from Session.
    // The are needed to detect changes of state, which in turn helps us to
    // update display items only if they have changed. Whether this is really
    // useful is to be determined. The cache for the activation state is gone.
    vec<int> metaInfoNums_; //!< indices of metadata items selected for display
};

void ExperimentModel::onClicked(const QModelIndex& cell) {
    int row = cell.row();
    int col = cell.column();
    if (row < 0 || row >= rowCount())
        return;
    if (col==1) {
        gSession->dataset().flipClusterActivation(row);
    }
    gSession->dataset().highlight().setCluster(row);
}

void ExperimentModel::onClustersChanged() {
    // Prefer the following over dataChanged because rowCount may have shrinked.
    // It resets the currentIndex so that arrow keys will start from row 0.
    // Therefore it is acceptable only here since we reset the highlight anyway.
    resetModel();
}

void ExperimentModel::onHighlight() {
    emit dataChanged(createIndex(0,0),createIndex(rowCount()-1,columnCount()-1));
}

void ExperimentModel::onActivated() {
    emit dataChanged(createIndex(0,1),createIndex(rowCount()-1,1));
}

void ExperimentModel::onMetaSelection() {
    beginResetModel(); // needed because columnCount may have shrinked
    metaInfoNums_.clear();
    const vec<bool>& selection = gSession->getMetaSelection();
    for_i (selection.count())
        if (selection.at(i))
            metaInfoNums_.append(i);
    emit dataChanged(createIndex(0,COL_ATTRS), createIndex(rowCount(),columnCount()));
    emit headerDataChanged(Qt::Horizontal, COL_ATTRS, columnCount());
    endResetModel();
}

QVariant ExperimentModel::data(const QModelIndex& index, int role) const {
    int row = index.row();
    if (row < 0 || row >= rowCount())
        return {};
    const Cluster& cluster = gSession->dataset().clusterAt(row);
    int col = index.column();
    switch (role) {
    case Qt::DisplayRole: {
        if (col==COL_NUMBER) {
            QString ret = QString::number(cluster.totalOffset()+1);
            if (cluster.count()>1)
                ret += "-" + QString::number(cluster.totalOffset()+cluster.count());
            return ret;
        } else if (col>=COL_ATTRS && col < COL_ATTRS+metaCount()) {
            return cluster.avgeMetadata()->attributeStrValue(
                metaInfoNums_.at(col-COL_ATTRS));
        } else
            return {};
    }
    case Qt::ToolTipRole: {
        QString ret;
        if (cluster.count()>1) {
            ret = QString("Measurements %1..%2 are numbers %3..%4 in file %5")
                .arg(cluster.totalOffset()+1)
                .arg(cluster.totalOffset()+cluster.count())
                .arg(cluster.offset()+1)
                .arg(cluster.offset()+cluster.count())
                .arg(cluster.file().name());
        } else {
            ret = QString("Measurement %1 is number %2 in file %3")
                .arg(cluster.totalOffset()+1)
                .arg(cluster.offset()+1)
                .arg(cluster.file().name());
        }
        ret += ".";
        if (cluster.isIncomplete())
            ret += QString("\nThis cluster has only %1 elements, while the binning factor is %2.")
                .arg(cluster.count())
                .arg(gSession->dataset().binning());
        return ret;
    }
    case Qt::ForegroundRole: {
        if (col==COL_NUMBER && cluster.count()>1 &&
            (cluster.isIncomplete()))
            return QColor(Qt::red);
        return QColor(Qt::black);
    }
    case Qt::BackgroundRole: {
        if (row==gSession->dataset().highlight().clusterIndex())
            return QColor(Qt::cyan);
        return QColor(Qt::white);
    }
    case Qt::CheckStateRole: {
        if (col==COL_CHECK) {
            return cluster.isActivated() ? Qt::Checked : Qt::Unchecked;
        }
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

class ExperimentView : public ListView { // < QTreeView < QAbstractItemView
public:
    ExperimentView();

private:
    void currentChanged(QModelIndex const&, QModelIndex const&) override final;
    void onKey(int row);
    void onClustersChanged();
    void onHighlight();
    void onActivated();
    void onMetaSelection();
    void updateScroll();
    int sizeHintForColumn(int) const override final;
    ExperimentModel* model_;
};

ExperimentView::ExperimentView() : ListView() {
    setHeaderHidden(true);
    setSelectionMode(QAbstractItemView::NoSelection);
    model_ = new ExperimentModel();
    setModel(model_);
    connect(gSession, &Session::sigClusters, this, &ExperimentView::onClustersChanged);
    connect(gSession, &Session::sigHighlight, this, &ExperimentView::onHighlight);
    connect(gSession, &Session::sigActivated, this, &ExperimentView::onActivated);
    connect(gSession, &Session::sigMetaSelection, this, &ExperimentView::onMetaSelection);
    connect(this, &ExperimentView::clicked, model_, &ExperimentModel::onClicked);
}

//! Overrides QAbstractItemView. This slot is called when a new item becomes the current item.
void ExperimentView::currentChanged(QModelIndex const& current, QModelIndex const& previous) {
    if (!gSession->dataset().countFiles())
        return;
    if (current.row()==gSession->dataset().highlight().clusterIndex())
        return; // the following would prevent execution of "onClicked"
    gSession->dataset().highlight().setCluster(current.row());
    updateScroll();
}

void ExperimentView::onClustersChanged() {
    model_->onClustersChanged();
    updateScroll();
}

void ExperimentView::onHighlight() {
    model_->onHighlight();
    updateScroll();
}

void ExperimentView::onActivated() {
    model_->onActivated();
    updateScroll();
}

void ExperimentView::onMetaSelection() {
    model_->onMetaSelection();
    setHeaderHidden(model_->metaCount()==0);
}

void ExperimentView::updateScroll() {
    int row = gSession->dataset().highlight().clusterIndex();
    if (row>=0)
        scrollTo(model_->index(row,0));
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
//  class ExperimentControls
// ************************************************************************** //

//! Row of controls to combine Measurement|s into Cluster|s.

class ExperimentControls : public QWidget {
public:
    ExperimentControls();
};

ExperimentControls::ExperimentControls() {

    auto layout = newQ::HBoxLayout();
    setLayout(layout);

    // 'combine' control
    layout->addWidget(newQ::Label("", "combine:"));
    auto combineMeasurements = newQ::SpinBox("combineMeasurements", 4, false, 1);
    layout->addWidget(combineMeasurements);
    combineMeasurements->setToolTip("Combine and average number of cluster");
    connect(combineMeasurements, _SLOT_(QSpinBox, valueChanged, int),
            [this](int num) { gSession->dataset().setBinning(num); });

    // 'if incomplete' control
    layout->addStretch(1);
    auto ifIncompleteLabel = newQ::Label("ifIncompleteLabel", "if incomplete:");
    layout->addWidget(ifIncompleteLabel);
    auto ifIncomplete = new QComboBox;
    ifIncomplete->addItems({"keep", "drop"});
    layout->addWidget(ifIncomplete);
    connect(ifIncomplete, _SLOT_(QComboBox, currentIndexChanged, int),
            [this](int index) { gSession->dataset().setDropIncomplete(index); });

    // back connection, to change controls from saved session
    connect(gSession, &Session::sigClusters, [=]() {
            combineMeasurements->setValue(gSession->dataset().binning());
            if (gSession->dataset().hasIncomplete()) {
                ifIncompleteLabel->show(); ifIncomplete->show();
            } else {
                ifIncompleteLabel->hide(); ifIncomplete->hide();
            }
        });
}

// ************************************************************************** //
//  class SubframeMeasurements
// ************************************************************************** //

SubframeMeasurements::SubframeMeasurements() : DockWidget("Measurements", "dock-cluster") {
    box_->addWidget(new ExperimentView()); // list of Cluster|s
    box_->addWidget(new ExperimentControls()); // controls row
}
