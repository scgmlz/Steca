// ************************************************************************** //
//
//  STeCa2:    StressTextureCalculator ver. 2
//
//! @file      views.cpp
//!
//! @homepage  http://apps.jcns.fz-juelich.de/steca2
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016
//! @authors   Scientific Computing Group at MLZ Garching
//! @authors   Rebecca Brydon, Jan Burle,  Antti Soininen
//! @authors   Based on the original STeCa by Christian Randau
//
// ************************************************************************** //

#include "views.h"
#include "calc/calc_reflection.h"
#include "thehub.h"

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
  selectRow(qMin(row, model()->rowCount() - 1));
}

void ListView::selectRow(uint row) {
  setCurrentIndex(model()->index(row, 0));
}

//------------------------------------------------------------------------------

MultiListView::MultiListView(TheHub& hub) : super(hub) {
  setSelectionMode(ExtendedSelection);
}

void MultiListView::selectRows(uint_vec rows) {
  auto m    = model();
  uint cols = m->columnCount();

  QItemSelection is;
  for (int row : rows)
    is.append(QItemSelectionRange(m->index(row, 0), m->index(row, cols - 1)));

  selectionModel()->select(is, QItemSelectionModel::ClearAndSelect);
}

//------------------------------------------------------------------------------
}}
// eof
