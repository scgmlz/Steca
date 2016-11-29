/*******************************************************************************
 * STeCa2 - StressTextureCalculator ver. 2
 *
 * Copyright (C) 2016 Forschungszentrum Jülich GmbH 2016
 *
 * This program is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the Free
 * Software Foundation, either version 3 of the License, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE.
 *
 * See the COPYING and AUTHORS files for more details.
 ******************************************************************************/

#include "dock_metadata.h"
#include "thehub.h"
#include "views.h"
#include <QHeaderView>

namespace gui { namespace panel {
//------------------------------------------------------------------------------

class MetadataView : public views::ListView {
  CLS(MetadataView) SUPER(views::ListView)
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

//------------------------------------------------------------------------------

DockMetadata::DockMetadata(TheHub& hub)
: super("Metadata", "dock-metadata", Qt::Vertical), RefHub(hub)
{
  box_->addWidget((metadataView_ = new MetadataView(hub)));
}

//------------------------------------------------------------------------------
}}
// eof
