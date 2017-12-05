// ************************************************************************** //
//
//  Steca2: stress and texture calculator
//
//! @file      gui/views.cpp
//! @brief     Implements ...
//!
//! @homepage  https://github.com/scgmlz/Steca2
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2017
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
// ************************************************************************** //


#include "views.h"
#include "calc/calc_reflection.h"
#include "thehub.h"
#include <QCheckBox>

namespace gui { namespace views {

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


}}
