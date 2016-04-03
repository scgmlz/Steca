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

FileViewModel::FileViewModel(TheHub& theHub): TableModel(theHub) {
}

int FileViewModel::columnCount(rcIndex) const {
  return 1 + DCOL;
}

int FileViewModel::rowCount(rcIndex) const {
  return theHub.numFiles(true);
}

QVariant FileViewModel::data(rcIndex index,int role) const {
  auto row = index.row(), rowCnt = rowCount();
  if (row < 0 || rowCnt <= row) return EMPTY_VAR;

  // the correction file (if any) is dislayed in the last row
  bool isCorrFile = theHub.hasCorrFile() && row+1 == rowCnt;

  switch (role) {
    case Qt::DisplayRole: {
      str s = theHub.fileName(row);
      static str const format("Corr: %1");
      return isCorrFile ? format.arg(s) : s;
    }
    case GetFileRole:
      return QVariant::fromValue<core::shp_File>(theHub.getFile(row));
    case IsCorrFileRole:
      return isCorrFile;
    default:
      return EMPTY_VAR;
  }
}

void FileViewModel::remFile(uint i) {
  theHub.remFile(i);
}

//------------------------------------------------------------------------------

DatasetViewModel::DatasetViewModel(TheHub& theHub)
: super(theHub), file(nullptr), metaInfo(nullptr) {
}

int DatasetViewModel::columnCount(rcIndex) const {
  return COL_ATTRS + metaInfoNums.count();
}

int DatasetViewModel::rowCount(rcIndex) const {
  return file ? file->numDatasets() : 0;
}

QVariant DatasetViewModel::data(rcIndex index,int role) const {
  if (!file) return EMPTY_VAR;

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
      return file->getDataset(row)->getAttributeStrValue(metaInfoNums[col-COL_ATTRS]);
    }
  }

  case GetDatasetRole:
    return QVariant::fromValue<core::shp_Dataset>(file->getDataset(row));
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
    return core::Dataset::getAttributeTag(metaInfoNums[col-COL_ATTRS]);
  }
}

void DatasetViewModel::setFile(core::shp_File file_) {
  beginResetModel();
  file = file_;
  endResetModel();
}

void DatasetViewModel::showMetaInfo(checkedinfo_vec const& infos_) {
  beginResetModel();

  metaInfoNums.clear();

  if ((metaInfo = &infos_)) {
    for_i (metaInfo->count()) {
      auto &item = metaInfo->at(i);
      if (item.cb->isChecked())
        metaInfoNums.append(i);
    }
  }

 endResetModel();
}

//------------------------------------------------------------------------------

ReflectionViewModel::ReflectionViewModel(TheHub& theHub): super(theHub) {
}

int ReflectionViewModel::columnCount(rcIndex) const {
  return NUM_COLUMNS;
}

int ReflectionViewModel::rowCount(rcIndex) const {
  return theHub.getReflections().count();
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
      return str().setNum(row+1);
    case COL_TYPE:
      return core::Reflection::reflType(theHub.getReflections()[row]->getType());
    default:
      return EMPTY_VAR;
    }
  }

  case GetDatasetRole:
    return QVariant::fromValue<core::shp_Reflection>(theHub.getReflections()[row]);
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
  theHub.addReflection(type);
}

void ReflectionViewModel::remReflection(uint i) {
  theHub.remReflection(i);
}

//------------------------------------------------------------------------------
}
// eof
