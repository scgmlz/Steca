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
#include <QHeaderView>

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

void MultiListView::updateNoSelection() {
  Model *m = dynamic_cast<Model*>(model());
  if (m) m->signalReset();
  clearSelection();
}

void MultiListView::clearSelection() {
  selectionModel()->clearSelection();
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

FilesView::FilesView(TheHub& hub): super(hub)
, model_(hub.fileViewModel), selfSignal_(false) {
  setModel(&model_);
  header()->hide();

  connect(hub_.actions.remFile, &QAction::triggered, [this]() {
    removeSelected();
    updateNoSelection();
  });

  ON_HUB_SIGNAL(filesChanged,  () {
    updateNoSelection();
  })

  ON_HUB_SIGNAL(filesSelected, () {
    if (!selfSignal_)
      selectRows(hub_.collectedFromFiles());
  })
}

class bool_lock {
public:
  bool_lock(bool &val): val_(val) {
    val_ = true;
  }

 ~bool_lock() {
    val_ = false;
  }

private:
  bool &val_;
};

void FilesView::selectionChanged(QItemSelection const& selected, QItemSelection const& deselected) {
  super::selectionChanged(selected,deselected);

  uint_vec fileNums;
  for (auto &index: selectedIndexes()) {
    ASSERT(0 <= index.row());
    fileNums.append(index.row());
  }

  bool_lock __(selfSignal_); // REVIEW

  hub_.collectDatasetsFromFiles(fileNums);
}

void FilesView::removeSelected() {
  auto indexes = selectedIndexes();

  // backwards
  for (uint i = indexes.count(); i-- > 0; )
    model_.remFile(indexes.at(i).row());

  clearSelection();
  model_.signalReset();

  hub_.collectDatasetsFromFiles({});
}

//------------------------------------------------------------------------------

DatasetView::DatasetView(TheHub& hub): super(hub), model_(hub.datasetViewModel) {
  setModel(&model_);

  ON_HUB_SIGNAL(datasetsChanged, () {
    model_.signalReset();
     setCurrentIndex(model_.index(0,0));
  })

  connect(&model_, &QAbstractItemModel::modelReset, [this]() {
    for_i (model_.columnCount())
      resizeColumnToContents(i);
  });
}

void DatasetView::selectionChanged(QItemSelection const& selected, QItemSelection const& deselected) {
  super::selectionChanged(selected,deselected);

  auto indexes = selected.indexes();
  hub_.tellSelectedDataset(indexes.isEmpty()
    ? core::shp_Dataset()
    : model_.data(indexes.first(), Model::GetDatasetRole).value<core::shp_Dataset>());
}

//------------------------------------------------------------------------------

ReflectionView::ReflectionView(TheHub& hub)
: super(hub), model_(hub.reflectionViewModel) {
  setModel(&model_);
  for_i (model_.columnCount())
    resizeColumnToContents(i);
}

void ReflectionView::addReflection(int type) {
  using eType = core::ePeakType;
  model_.addReflection((eType)qBound(0,type,(int)eType::NUM_TYPES)); // make safe
  updateSingleSelection();
}

void ReflectionView::removeSelected() {
  int row = currentIndex().row();
  if (row<0 || model_.rowCount() <= row) return;

  model_.remReflection(row);
  updateSingleSelection();
}

bool ReflectionView::hasReflections() const {
  return model_.rowCount() > 0;
}

core::shp_Reflection ReflectionView::selectedReflection() const {
  auto indexes = selectionModel()->selectedIndexes();
  if (indexes.isEmpty())
    return core::shp_Reflection();
  return model_.data(indexes.first(), Model::GetDatasetRole).value<core::shp_Reflection>();
}

void ReflectionView::updateSingleSelection() {
  super::updateSingleSelection();
  hub_.actions.remReflection->setEnabled(hasReflections());
}

void ReflectionView::selectionChanged(QItemSelection const& selected, QItemSelection const& deselected) {
  super::selectionChanged(selected,deselected);

  auto indexes = selected.indexes();
  hub_.tellSelectedReflection(indexes.isEmpty()
    ? core::shp_Reflection()
    : model_.data(indexes.first(), Model::GetDatasetRole).value<core::shp_Reflection>());
}

//------------------------------------------------------------------------------
}}
// eof
