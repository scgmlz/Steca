//  ***********************************************************************************************
//
//  Steca: stress and texture calculator
//
//! @file      gui/panels/subframe_clusters.cpp
//! @brief     Implements class SubframeClusters, with local model and view
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
//  ***********************************************************************************************

#include "gui/panels/subframe_clusters.h"
#include "core/session.h"
#include "qcr/widgets/tables.h"
//#include "qcr/base/debug.h"

//  ***********************************************************************************************
//! @class ActiveClustersModel (local scope)

//! The model for ActiveClustersView.

class ActiveClustersModel : public CheckTableModel { // < QAbstractTableModel < QAbstractItemModel
public:
    ActiveClustersModel() : CheckTableModel("measurement") {}
    int columnCount() const final {
        return COL_ATTRS + gSession->params.smallMetaSelection.numSelected(); }

    enum { COL_CHECK=1, COL_NUMBER, COL_ATTRS };

private:
    int highlighted() const final;
    void onHighlight(int row) final { gSession->dataset.highlight().setCluster(row); }
    bool activated(int row) const { return gSession->dataset.allClusters.at(row)->isActivated(); }
    void setActivated(int row, bool on) { gSession->dataset.setClusterActivation(row, on); }

    int rowCount() const final { return gSession->dataset.allClusters.size(); }

    QVariant data(const QModelIndex&, int) const final;
    QVariant headerData(int, Qt::Orientation, int) const final;
};

int ActiveClustersModel::highlighted() const
{
    const Cluster* c = gSession->currentCluster();
    return c ? c->index() : -1;
}

QVariant ActiveClustersModel::data(const QModelIndex& index, int role) const
{
    int row = index.row();
    if (row < 0 || row >= rowCount())
        return {};
    const Cluster& cluster = *gSession->dataset.allClusters.at(row);
    int col = index.column();
    switch (role) {
    case Qt::DisplayRole: {
        if (col==COL_NUMBER) {
            QString ret = QString::number(cluster.totalOffset()+1);
            if (cluster.size()>1)
                ret += "-" + QString::number(cluster.totalOffset()+cluster.size());
            return ret;
        } else if (col>=COL_ATTRS &&
                   col < COL_ATTRS+gSession->params.smallMetaSelection.numSelected()) {
            return cluster.avgMetadata().attributeStrValue(
                gSession->params.smallMetaSelection.selectedOf(col-COL_ATTRS));
        } else
            return {};
    }
    case Qt::ToolTipRole: {
        QString ret;
        if (cluster.size()>1) {
            ret = QString("Measurements %1..%2 are numbers %3..%4 in file %5")
                .arg(cluster.totalOffset()+1)
                .arg(cluster.totalOffset()+cluster.size())
                .arg(cluster.offset()+1)
                .arg(cluster.offset()+cluster.size())
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
                .arg(cluster.size())
                .arg(gSession->dataset.binning.val());
        return ret;
    }
    case Qt::ForegroundRole: {
        if (col==COL_NUMBER && cluster.size()>1 &&
            (cluster.isIncomplete()))
            return QColor(Qt::red);
        return QColor(Qt::black);
    }
    case Qt::BackgroundRole: {
        if (row==highlighted())
            return QColor(Qt::cyan);
        return QColor(Qt::white);
    }
    case Qt::CheckStateRole: {
        if (col==COL_CHECK)
            return activated(row) ? Qt::Checked : Qt::Unchecked;
        return {};
    }
    default:
        return {};
    }
}

QVariant ActiveClustersModel::headerData(int col, Qt::Orientation ori, int role) const
{
    if (ori!=Qt::Horizontal)
        return {};
    if (role != Qt::DisplayRole)
        return {};
    if (col==COL_NUMBER)
        return "#";
    else if (col>=COL_ATTRS &&
             col < COL_ATTRS+gSession->params.smallMetaSelection.numSelected())
        return Metadata::attributeTag(
            gSession->params.smallMetaSelection.selectedOf(col-COL_ATTRS), false);
    return {};
}


//  ***********************************************************************************************
//! @class ActiveClustersView (local scope)

//! Main item in SubframeMeasurement: View and control of measurements list.

class ActiveClustersView : public CheckTableView { // < QTreeView < QAbstractItemView
public:
    ActiveClustersView();
private:
    ActiveClustersModel* model() { return static_cast<ActiveClustersModel*>(model_); }
    void onData() override;
};

ActiveClustersView::ActiveClustersView()
    : CheckTableView(new ActiveClustersModel())
{
    setSelectionMode(QAbstractItemView::NoSelection);
    onData();
}

void ActiveClustersView::onData()
{
    setHeaderHidden(!gSession->params.smallMetaSelection.numSelected());
    setColumnWidth(0, 0);
    setColumnWidth(1,  3*dWidth());
    for (int i=2; i<model_->columnCount(); ++i)
        setColumnWidth(i, 7.*dWidth());
    model_->refreshModel();
    emit model_->layoutChanged();
    updateScroll();
}


//  ***********************************************************************************************
//! @class SubframeClusters

SubframeClusters::SubframeClusters()
    : QcrDockWidget("measurements")
{
    setFeatures(DockWidgetMovable);
    setWindowTitle("Measurements");
    setWidget(new ActiveClustersView()); // list of Cluster|s
    setRemake([this](){setEnabled(gSession->hasData());});
}
