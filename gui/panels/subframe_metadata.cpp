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

#include "gui/panels/subframe_metadata.h"
#include "core/session.h"
#include "qcr/widgets/tables.h"

//  ***********************************************************************************************
//! @class MetabigtableModel (local scope)

//! The model for MetadatView.

class MetabigtableModel : public CheckTableModel {
public:
    MetabigtableModel() : CheckTableModel("meta") {}

    enum { COL_CHECK = 1, COL_TAG, COL_VALUE, NUM_COLUMNS };

private:
    int highlighted() const final { return highlighted_; }
    void onHighlight(int i) final { highlighted_ = i; }
    bool activated(int row) const { return gSession->params.smallMetaSelection.isSelected(row); }
    void setActivated(int row, bool on) { gSession->params.smallMetaSelection.set(row, on); }

    int columnCount() const final { return NUM_COLUMNS; }
    int rowCount() const final { return Metadata::numAttributes(false); }

    QVariant data(const QModelIndex&, int) const;
    QVariant headerData(int, Qt::Orientation, int) const { return {}; }

    int highlighted_ {0};
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
        switch (col) {
        case COL_TAG:
            return Metadata::attributeTag(row, false);
        case COL_VALUE:
            const Cluster* highlight = gSession->currentCluster();
            if (!highlight)
                return "-";
            return highlight->avgMetadata().attributeStrValue(row);
        }
        return "";
    case Qt::BackgroundRole:
        return QColor(Qt::white);
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
    MetabigtableModel* model() { return static_cast<MetabigtableModel*>(model_); }
};

MetabigtableView::MetabigtableView()
    : CheckTableView(new MetabigtableModel())
{
    setColumnWidth(0, 0);
    setColumnWidth(1,  .5*mWidth());
    setColumnWidth(2, 6. *mWidth());
    setColumnWidth(3, 7.5*mWidth());
}

//  ***********************************************************************************************
//! @class SubframeMetadata

SubframeMetadata::SubframeMetadata()
    : QcrDockWidget("metadata")
{
    for (int i=0; i<Metadata::size(); ++i)
        gSession->params.smallMetaSelection.vec.push_back({false});
    setFeatures(DockWidgetMovable);
    setWindowTitle("Metadata");
    setWidget(new MetabigtableView());
    setRemake([this](){setEnabled(gSession->hasData());});
}
