// ************************************************************************** //
//
//  STeCa2:    StressTexCalculator ver. 2
//
//! @file      models.cpp
//!
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016
//! @authors   Scientific Computing Group at MLZ Garching
//! @authors   Original version: Christian Randau
//! @authors   Version 2: Antti Soininen, Jan Burle, Rebecca Brydon
//
// ************************************************************************** //

#include "models.h"
#include "thehub.h"
#include <QCheckBox>

namespace models {
//------------------------------------------------------------------------------

FileViewModel::FileViewModel(gui::TheHub& theHub): TableModel(theHub) {
}

int FileViewModel::columnCount(rcIndex) const {
  return 1 + DCOL;
}

int FileViewModel::rowCount(rcIndex) const {
  return _theHub.numFiles(true);
}

QVariant FileViewModel::data(rcIndex index,int role) const {
  auto row = index.row(), rowCnt = rowCount();
  if (row < 0 || rowCnt <= row) return EMPTY_VAR;

  // the correction file (if any) is dislayed in the last row
  bool isCorrFile = _theHub.hasCorrFile() && row+1 == rowCnt;

  switch (role) {
    case Qt::DisplayRole: {
      str s = _theHub.fileName(row);
      static str const format("Corr: %1");
      return isCorrFile ? format.arg(s) : s;
    }
    case GetFileRole:
      return QVariant::fromValue<core::shp_File>(_theHub.getFile(row));
    case IsCorrFileRole:
      return isCorrFile;
    default:
      return EMPTY_VAR;
  }
}

void FileViewModel::remFile(uint i) {
  _theHub.remFile(i);
}

//------------------------------------------------------------------------------

DatasetViewModel::DatasetViewModel(gui::TheHub& theHub)
: super(theHub), _file(nullptr), _metaInfo(nullptr) {
}

int DatasetViewModel::columnCount(rcIndex) const {
  return COL_ATTRS + _metaInfoNums.count();
}

int DatasetViewModel::rowCount(rcIndex) const {
  return _file ? _file->datasets().count() : 0;
}

QVariant DatasetViewModel::data(rcIndex index,int role) const {
  if (!_file) return EMPTY_VAR;

  int row = index.row();
  if (row < 0 || rowCount() <= row) return EMPTY_VAR;

  switch (role) {
  case Qt::DisplayRole: {
    int col = index.column();
    if (col < DCOL || columnCount() <= col) return EMPTY_VAR;

    switch (col) {
    case COL_NUMBER:
      return str::number(row+1);
    default:
      return _file->datasets().at(row)->attributeStrValue(_metaInfoNums[col-COL_ATTRS]);
    }
  }

  case GetDatasetRole:
    return QVariant::fromValue<core::shp_Dataset>(_file->datasets().at(row));
  default:
    return EMPTY_VAR;
  }
}

QVariant DatasetViewModel::headerData(int col, Qt::Orientation, int role) const {
  if (Qt::DisplayRole != role || col < DCOL || columnCount() <= col)
    return EMPTY_VAR;

  switch (col) {
  case COL_NUMBER:
    return "#";
  default:
    return core::Dataset::attributeTag(_metaInfoNums[col-COL_ATTRS]);
  }
}

void DatasetViewModel::setFile(core::shp_File file_) {
  beginResetModel();
  _file = file_;
  endResetModel();
}

void DatasetViewModel::showMetaInfo(checkedinfo_vec const& infos_) {
  beginResetModel();

  _metaInfoNums.clear();

  if ((_metaInfo = &infos_)) {
    for_i (_metaInfo->count()) {
      auto &item = _metaInfo->at(i);
      if (item.cb->isChecked())
        _metaInfoNums.append(i);
    }
  }

 endResetModel();
}

//------------------------------------------------------------------------------

ReflectionViewModel::ReflectionViewModel(gui::TheHub& theHub): super(theHub) {
}

int ReflectionViewModel::columnCount(rcIndex) const {
  return NUM_COLUMNS;
}

int ReflectionViewModel::rowCount(rcIndex) const {
  return _theHub.reflections().count();
}

QVariant ReflectionViewModel::data(rcIndex index, int role) const {
  int row = index.row();
  if (row < 0 || rowCount() <= row) return EMPTY_VAR;

  switch (role) {
  case Qt::DisplayRole: {
    int col = index.column();
    if (col < DCOL) return EMPTY_VAR;

    switch (col) {
    case COL_ID:
      return str::number(row+1);
    case COL_TYPE:
      return core::Reflection::typeTag(_theHub.reflections()[row]->type());
    default:
      return EMPTY_VAR;
    }
  }

  case GetDatasetRole:
    return QVariant::fromValue<core::shp_Reflection>(_theHub.reflections()[row]);
  default:
    return EMPTY_VAR;
  }
}

QVariant ReflectionViewModel::headerData(int col, Qt::Orientation, int role) const {
  if (Qt::DisplayRole == role && COL_ID == col)
    return "#";
  else
    return EMPTY_VAR;
}

void ReflectionViewModel::addReflection(core::ePeakType type) {
  _theHub.addReflection(type);
}

void ReflectionViewModel::remReflection(uint i) {
  _theHub.remReflection(i);
}

//------------------------------------------------------------------------------
}
// eof
