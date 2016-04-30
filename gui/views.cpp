// ************************************************************************** //
//
//  STeCa2:    StressTexCalculator ver. 2
//
//! @file      views.cpp
//!
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016
//! @authors   Scientific Computing Group at MLZ Garching
//! @authors   Original version: Christian Randau
//! @authors   Version 2: Antti Soininen, Jan Burle, Rebecca Brydon
//
// ************************************************************************** //

#include "views.h"
#include "thehub.h"
#include "core_reflection.h"

namespace gui { namespace views {
//-----------------------------------------------------------------------------

ListView::ListView(TheHub& hub): RefHub(hub) {
}

void ListView::updateSingleSelection() {
  Model *m = dynamic_cast<Model*>(model());
  if (m) {
    int row = currentIndex().row();
    m->signalReset();
    selectRow(qMin(row,m->rowCount()-1));
  }
}

void ListView::selectRow(uint row) {
  setCurrentIndex(model()->index(row,0));
}

//------------------------------------------------------------------------------

MultiListView::MultiListView(TheHub& hub): super(hub) {
  setSelectionMode(ExtendedSelection);
}

void MultiListView::selectRows(uint_vec rows) {
  auto m = model();
  uint cols = m->columnCount();

  QItemSelection is;
  for (int row: rows)
    is.append(QItemSelectionRange(m->index(row,0), m->index(row,cols-1)));

  selectionModel()->select(is,QItemSelectionModel::ClearAndSelect);
}

//------------------------------------------------------------------------------
}}
// eof
