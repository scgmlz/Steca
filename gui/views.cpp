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

#include "views.h"
#include "calc/calc_reflection.h"
#include "thehub.h"
#include <QCheckBox>

namespace gui { namespace views {
//-----------------------------------------------------------------------------

ListView::ListView(TheHub& hub) : RefHub(hub) {
}

void ListView::setModel(Model* model) {
  super::setModel(model);
  EXPECT(dynamic_cast<Model*>(super::model()))
}

void ListView::updateSingleSelection() {
  int row = currentIndex().row();
  model()->signalReset();
  selectRow(row);
}

void ListView::selectRow(int row) {
  setCurrentIndex(model()->index(row, 0));
}

//------------------------------------------------------------------------------

MultiListView::MultiListView(TheHub& hub) : super(hub) {
  setSelectionMode(ExtendedSelection);
}

void MultiListView::selectRows(uint_vec rows) {
  auto m   = model();
  int cols = m->columnCount();

  QItemSelection is;
  for (uint row : rows)
    is.append(QItemSelectionRange(m->index(to_i(row), 0),
                                  m->index(to_i(row), cols - 1)));

  selectionModel()->select(is, QItemSelectionModel::ClearAndSelect);
}

//------------------------------------------------------------------------------
}}
// eof

