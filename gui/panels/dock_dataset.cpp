/*******************************************************************************
 * REVIEW: STeCa2 - StressTextureCalculator ver. 2
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

#include "dock_dataset.h"
#include "gui_cfg.h"
#include "thehub.h"
#include "views.h"

namespace gui { namespace panel {
//------------------------------------------------------------------------------

class DatasetView : public views::ListView {
  CLASS(DatasetView) SUPER(views::ListView)
public:
  DatasetView(TheHub&);

protected:
  void currentChanged(QModelIndex const&, QModelIndex const&);

  using Model = models::DatasetsModel;
  Model* model() const { return static_cast<Model*>(super::model()); }
};

DatasetView::DatasetView(TheHub& hub) : super(hub) {
  setModel(&hub.datasetsModel);
  EXPECT(dynamic_cast<Model*>(super::model()))

  onSigDatasetsChanged([this]() {
    tellDatasetSelected(data::shp_Dataset()); // first de-select
    selectRow(0);
  });
}

void DatasetView::currentChanged(QModelIndex const& current,
                                 QModelIndex const& previous) {
  super::currentChanged(current, previous);
  tellDatasetSelected(model()->data(current, Model::GetDatasetRole)
                      .value<data::shp_Dataset>());
}

//------------------------------------------------------------------------------

DockDatasets::DockDatasets(TheHub& hub)
: super("Datasets", "dock-datasets", Qt::Vertical), RefHub(hub)
{
  box_->addWidget((datasetView_ = new DatasetView(hub)));

  auto h = hbox();
  box_->addLayout(h);

  h->addWidget(label("Combine:"));
  h->addWidget(combineDatasets_ = spinCell(gui_cfg::em4, 1));
  combineDatasets_->setToolTip("Combine and average number of datasets");

  connect(combineDatasets_, slot(QSpinBox,valueChanged,int), [this](int num) {
    hub_.combineDatasetsBy(pint(qMax(1, num)));
  });

  onSigDatasetsChanged([this]() {
    combineDatasets_->setValue(to_i(uint(hub_.datasetsGroupedBy())));
  });
}

//------------------------------------------------------------------------------
}}
// eof
