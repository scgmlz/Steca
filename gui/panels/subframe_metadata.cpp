// ************************************************************************** //
//
//  Steca: stress and texture calculator
//
//! @file      gui/panels/subframe_metadata.cpp
//! @brief     Implements class SubframeMetadata
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
// ************************************************************************** //

#include "gui/panels/subframe_metadata.h"
#include "gui/models.h"
#include "gui/thehub.h"
#include "gui/widgets/tree_views.h" // inheriting from

// ************************************************************************** //
//  local class MetadataView
// ************************************************************************** //

class MetadataView : public ListView {
public:
    MetadataView();

private:
    int sizeHintForColumn(int) const;
    MetadataModel* model() const { return static_cast<MetadataModel*>(ListView::model()); }
};

MetadataView::MetadataView() : ListView() {
    setModel(gHub->metadataModel);
    debug::ensure(dynamic_cast<MetadataModel*>(ListView::model()));
    connect(this, &MetadataView::clicked, [this](QModelIndex const& index) {
        model()->flipCheck(to_u(index.row()));
        emit gHub->sigMetatagsChosen(model()->rowsChecked());
    });
}

int MetadataView::sizeHintForColumn(int col) const {
    switch (col) {
    case MetadataModel::COL_CHECK:
        return fontMetrics().width('m');
    default:
        return ListView::sizeHintForColumn(col);
    }
}

// ************************************************************************** //
//  class SubframeMetadata
// ************************************************************************** //

SubframeMetadata::SubframeMetadata() : DockWidget("Metadata", "dock-metadata") {
    box_->addWidget((metadataView_ = new MetadataView()));
}
