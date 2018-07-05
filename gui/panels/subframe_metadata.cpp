//  ***********************************************************************************************
//
//  Steca: stress and texture calculator
//
//! @file      gui/panels/subframe_metadata.cpp
//! @brief     Implements class SubframeMetadata, with local model and view
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
//  ***********************************************************************************************

#include "core/session.h"
#include "gui/panels/subframe_metadata.h"
#include "qcr/widgets/tables.h"

//  ***********************************************************************************************
//! @class MetabigtableModel (local scope)

//! The model for MetadatView.

class MetabigtableModel : public CheckTableModel {
public:
    MetabigtableModel() : CheckTableModel("meta") {}

    void reset();

    int columnCount() const final { return NUM_COLUMNS; }
    int rowCount() const final { return Metadata::numAttributes(false); }
    int highlighted() const final { return 0; }// gSession->dataset().highlight().clusterIndex(); }
    void setHighlight(int i) final { ; } //gSession->dataset().highlight().setCluster(i); }
    bool activated(int row) const { return gSession->metaIsSelected(row); }
    void setActivated(int row, bool on) { gSession->setMetaSelected(row, on); }

    QVariant data(const QModelIndex&, int) const;
    QVariant headerData(int, Qt::Orientation, int) const { return {}; }

    enum { COL_CHECK = 1, COL_TAG, COL_VALUE, NUM_COLUMNS };
};

QVariant MetabigtableModel::data(const QModelIndex& index, int role) const
{
    int row = index.row();
    if (row < 0 || rowCount() <= row)
        return {};
    int col = index.column();
    switch (role) {
    case Qt::CheckStateRole:
        if (col==COL_CHECK)
            return activated(row) ? Qt::Checked : Qt::Unchecked;
        break;
    case Qt::DisplayRole:
        if (row==0 && col==1)
            qDebug() << "subframe Metadata: call to data()";
        switch (col) {
        case COL_TAG:
            return Metadata::attributeTag(row, false);
        case COL_VALUE:
            const Cluster* highlight = gSession->dataset().highlight().cluster();
            if (!highlight)
                return "-";
            return highlight->avgMetadata().attributeStrValue(row);
        }
        break;
    }
    return {};
}


//  ***********************************************************************************************
//! @class MetabigtableView (local scope)

//! Main item in SubframeMetadata: View and control the list of Metadata.

class MetabigtableView : public CheckTableView {
public:
    MetabigtableView();
private:
    void currentChanged(const QModelIndex& current, const QModelIndex&) override final {
        gotoCurrent(current); }
    int sizeHintForColumn(int) const final;
    MetabigtableModel* model() { return static_cast<MetabigtableModel*>(model_); }
};

MetabigtableView::MetabigtableView()
    : CheckTableView(new MetabigtableModel())
{
    // internal connection:
    connect(this, &MetabigtableView::clicked, model(), &CheckTableModel::onClicked);
}

int MetabigtableView::sizeHintForColumn(int col) const
{
    switch (col) {
    case MetabigtableModel::COL_CHECK:
        return 2*mWidth();
    default:
        return 3*mWidth();
    }
}

//  ***********************************************************************************************
//! @class SubframeMetadata

SubframeMetadata::SubframeMetadata()
    : QcrDockWidget("metadata")
{
    setFeatures(DockWidgetMovable);
    setWindowTitle("Metadata");
    setWidget(new MetabigtableView());
}
