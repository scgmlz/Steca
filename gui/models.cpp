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

namespace model {
//------------------------------------------------------------------------------

ModelBase::ModelBase(TheHub& theHub_): theHub(theHub_) {
}

//------------------------------------------------------------------------------

FileViewModel::FileViewModel(TheHub& theHub): ModelBase(theHub) {
}

uint FileViewModel::numFiles(bool withCorr) {
  return theHub.numFiles(withCorr);
}

void FileViewModel::remFile(uint i) {
  theHub.remFile(i);
}

int FileViewModel::columnCount(QModelIndex const&) const {
  return 2; // +1 for the hidden 0-th column
}

int FileViewModel::rowCount(QModelIndex const&) const {
  return theHub.numFiles(true);
}

QVariant FileViewModel::data(QModelIndex const& index,int role) const {
  auto row = index.row(), cnt = rowCount(index);
  if (row < 0 || row >= cnt) return QVariant();

  bool isCorrectionFile = theHub.hasCorrFile() && row+1 == cnt;

  switch (role) {
    case IsCorrectionFileRole:
      return isCorrectionFile;
    case Qt::DisplayRole: {
      str s = theHub.fileName(row);
      static str const Corr("Corr: ");
      if (isCorrectionFile) s = Corr + s;
      return s;
    }
    case GetFileRole:
      return QVariant::fromValue<core::shp_File>(theHub.getFile(row));
    default:
      return QVariant();
  }
}

void FileViewModel::signalReset() {
  beginResetModel();
  endResetModel();    // emits a signal to connected views
}

//------------------------------------------------------------------------------

DatasetViewModel::DatasetViewModel(TheHub& theHub)
: ModelBase(theHub), coreFile(nullptr), infoItems(nullptr) {
}

int DatasetViewModel::columnCount(QModelIndex const&) const {
  return attributeNums.count() + 2; // 1 for the hidden 0-th column, 1 for the sequence number
}

int DatasetViewModel::rowCount(QModelIndex const&) const {
  return coreFile ? coreFile->numDatasets() : 0;
}

QVariant DatasetViewModel::data(QModelIndex const& index,int role) const {
  if (!coreFile) return QVariant();

  int row = index.row(), col = index.column(), cnt = rowCount(index);
  if (row < 0 || row >= cnt || col < 0 || col-2 >= attributeNums.count()) return QVariant();

  switch (role) {
  case Qt::DisplayRole: {
    switch (col) {
    case 0:
      return QVariant();
    case 1:
      return str().setNum(row+1);
    default:
      return getDataset(row)->getAttributeStrValue(attributeNums[col-2]);
    }
  }
  case GetDatasetRole:
    return QVariant::fromValue<core::shp_Dataset>(getDataset(row));
  default:
    return QVariant();
  }
}

QVariant DatasetViewModel::headerData(int section, Qt::Orientation, int role) const {
  if (Qt::DisplayRole != role
      || section < 1 || section-2 >= attributeNums.count())
    return QVariant();

  switch (section) {
  case 1:
    return "#";
  default:
    return core::Dataset::getAttributeTag(attributeNums[section-2]);
  }
}

void DatasetViewModel::setCoreFile(core::shp_File coreFile_) {
  beginResetModel();
  coreFile = coreFile_;
  endResetModel();
}

void DatasetViewModel::setInfoItems(infoitem_vec const* infoItems_) {
  beginResetModel();
  infoItems = infoItems_;
  attributeNums.clear();
  if (infoItems) for_i (infoItems->count()) {
    auto &item = infoItems->at(i);
    if (item.cb->isChecked())
      attributeNums.append(i);
  }
  endResetModel();
}

core::shp_Dataset const& DatasetViewModel::getDataset(int row) const {
  return coreFile->getDataset(row);
}

//------------------------------------------------------------------------------

ReflectionViewModel::ReflectionViewModel(TheHub& theHub): ModelBase(theHub) {
}

int ReflectionViewModel::columnCount(QModelIndex const&) const {
  return NUM_COLUMNS;
}

int ReflectionViewModel::rowCount(QModelIndex const&) const {
  return theHub.getReflections().count();
}

QVariant ReflectionViewModel::data(QModelIndex const& index, int role) const {
  int row = index.row(), col = index.column(), rowCnt = rowCount(index);
  if (row < 0 || row >= rowCnt || col < 0 || col >= NUM_COLUMNS) return QVariant();

  switch (role) {
  case Qt::DisplayRole: {
    switch (col) {
    case COLUMN_ID:
      return str().setNum(row+1);
    case COLUMN_TYPE:
      return core::Reflection::reflType(theHub.getReflections()[row]->getType());
    default:
      return QVariant();
    }
  }
  case GetDatasetRole:
    return QVariant::fromValue<core::shp_Reflection>(theHub.getReflections()[row]);
  default:
    return QVariant();
  }

  return QVariant();
}

QVariant ReflectionViewModel::headerData(int section, Qt::Orientation, int role) const {
  if (Qt::DisplayRole == role && COLUMN_ID==section)
    return "#";

  return QVariant();
}

void ReflectionViewModel::addReflection(core::ePeakType type) {
  theHub.addReflection(type);
}

void ReflectionViewModel::remReflection(uint i) {
  theHub.remReflection(i);
}

void ReflectionViewModel::signalReset() {
  beginResetModel();
  endResetModel();    // emits a signal to connected views
}

//------------------------------------------------------------------------------
}
// eof
