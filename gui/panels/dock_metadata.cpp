// ************************************************************************** //
//
//  Steca2: stress and texture calculator
//
//! @file      gui/panels/dock_metadata.cpp
//! @brief     Implements class DockMetadata
//!
//! @homepage  https://github.com/scgmlz/Steca2
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2017
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
// ************************************************************************** //

#include "dock_metadata.h"
#include "widgets/tree_views.h" // inheriting from
#include "thehub.h"

namespace gui {
namespace panel {

class MetadataView : public ListView {
public:
    using Model = MetadataModel;
    MetadataView();

protected:
    int sizeHintForColumn(int) const;

private:
    Model* model() const { return static_cast<Model*>(ListView::model()); }
};

MetadataView::MetadataView() : ListView() {
    setModel(gHub->metadataModel);
    debug::ensure(dynamic_cast<Model*>(ListView::model()));
    connect(this, &MetadataView::clicked, [this](QModelIndex const& index) {
        model()->flipCheck(to_u(index.row()));
        gHub->datasequenceModel->showMetaInfo(model()->rowsChecked()); // REVIEW signal instead?
    });
}

int MetadataView::sizeHintForColumn(int col) const {
    switch (col) {
    case Model::COL_CHECK:
        return fontMetrics().width('m');
    default:
        return ListView::sizeHintForColumn(col);
    }
}

DockMetadata::DockMetadata()
    : DockWidget("Metadata", "dock-metadata", Qt::Vertical) {
    box_->addWidget((metadataView_ = new MetadataView()));
}

} // namespace panel
} // namespace gui
