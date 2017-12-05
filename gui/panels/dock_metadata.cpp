// ************************************************************************** //
//
//  Steca2: stress and texture calculator
//
//! @file      gui/panels/dock_metadata.cpp
//! @brief     Implements ...
//!
//! @homepage  https://github.com/scgmlz/Steca2
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2017
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
// ************************************************************************** //


#include "dock_metadata.h"
#include "thehub.h"
#include "views.h"
#include <QHeaderView>

namespace gui { namespace panel {

class MetadataView : public views::ListView {
  CLASS(MetadataView) SUPER(views::ListView)
public:
  using Model = models::MetadataModel;
  MetadataView(TheHub&);

protected:
  int sizeHintForColumn(int) const;

private:
  Model* model() const {
    return static_cast<Model*>(super::model());
  }
};

MetadataView::MetadataView(TheHub& hub) : super(hub) {
  setModel(&hub.metadataModel);
  EXPECT(dynamic_cast<Model*>(super::model()))

  connect(this, &MetadataView::clicked, [this](QModelIndex const& index) {
    model()->flipCheck(to_u(index.row()));
    hub_.datasetsModel.showMetaInfo(model()->rowsChecked()); // REVIEW signal instead?
  });
}

int MetadataView::sizeHintForColumn(int col) const {
  switch (col) {
  case Model::COL_CHECK:
    return mWidth(this);
  default:
    return super::sizeHintForColumn(col);
  }
}


DockMetadata::DockMetadata(TheHub& hub)
: super("Metadata", "dock-metadata", Qt::Vertical), RefHub(hub)
{
  box_->addWidget((metadataView_ = new MetadataView(hub)));
}


}}
