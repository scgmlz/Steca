// ************************************************************************** //
//
//  STeCa2:    StressTextureCalculator ver. 2
//
//! @file      models.cpp
//!
//! @homepage  http://apps.jcns.fz-juelich.de/steca2
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016
//! @authors   Scientific Computing Group at MLZ Garching
//! @authors   Rebecca Brydon, Jan Burle,  Antti Soininen
//! @authors   Based on the original STeCa by Christian Randau
//
// ************************************************************************** //

#include "models.h"
#include "calc/calc_reflection.h"
#include "thehub.h"
#include <QCheckBox>

namespace models {
//------------------------------------------------------------------------------

FilesModel::FilesModel(gui::TheHub& hub) : TableModel(hub) {
  onSigFilesChanged([this]() {
    signalReset();
  });
}

int FilesModel::columnCount(rcIndex) const {
  return 1 + DCOL;
}

int FilesModel::rowCount(rcIndex) const {
  return to_i(hub_.numFiles());
}

QVariant FilesModel::data(rcIndex index, int role) const {
  auto row = index.row(), rowCnt = rowCount();
  if (row < 0 || rowCnt <= row)
    return EMPTY_VAR;

  switch (role) {
  case Qt::DisplayRole:
    return hub_.fileName(to_u(row));
  case GetFileRole:
    return QVariant::fromValue<data::shp_File>(hub_.getFile(to_u(row)));
  default:
    return EMPTY_VAR;
  }
}

void FilesModel::remFile(uint i) {
  hub_.remFile(i);
}

//------------------------------------------------------------------------------

DatasetsModel::DatasetsModel(gui::TheHub& hub)
: super(hub), datasets_(hub.collectedDatasets()), metaInfo_(nullptr)
{
  onSigDatasetsChanged([this]() {
    signalReset();
  });
}

int DatasetsModel::columnCount(rcIndex) const {
  return COL_ATTRS + to_i(metaInfoNums_.count());
}

int DatasetsModel::rowCount(rcIndex) const {
  return to_i(datasets_.count());
}

QVariant DatasetsModel::data(rcIndex index, int role) const {
  int row = index.row();
  if (row < 0 || rowCount() <= row)
    return EMPTY_VAR;

  switch (role) {
  case Qt::DisplayRole: {
    int col = index.column();
    if (col < DCOL || columnCount() <= col)
      return EMPTY_VAR;

    switch (col) {
    case COL_NUMBER:
      return hub_.collectedDatasetsTags().at(to_u(row));
    default:
      return datasets_.at(to_u(row))->metadata()->attributeStrValue(
          metaInfoNums_.at(to_u(col - COL_ATTRS)));
    }
  }

  case GetDatasetRole:
    return QVariant::fromValue<data::shp_Dataset>(datasets_.at(to_u(row)));
  default:
    return EMPTY_VAR;
  }
}

QVariant DatasetsModel::headerData(int col, Qt::Orientation, int role) const {
  if (Qt::DisplayRole != role || col < DCOL || columnCount() <= col)
    return EMPTY_VAR;

  switch (col) {
  case COL_NUMBER:
    return "#";
  default:
    return data::Metadata::attributeTag(metaInfoNums_.at(to_u(col - COL_ATTRS)));
  }
}

void DatasetsModel::showMetaInfo(checkedinfo_vec::rc infos_) {
  beginResetModel();

  metaInfoNums_.clear();

  if ((metaInfo_ = &infos_)) {
    for_i (metaInfo_->count()) {
      auto& item = metaInfo_->at(i);
      if (item.cb->isChecked()) metaInfoNums_.append(i);
    }
  }

  endResetModel();
}

//------------------------------------------------------------------------------

ReflectionsModel::ReflectionsModel(gui::TheHub& hub) : super(hub) {
}

int ReflectionsModel::columnCount(rcIndex) const {
  return NUM_COLUMNS;
}

int ReflectionsModel::rowCount(rcIndex) const {
  return to_i(hub_.reflections().count());
}

str ReflectionsModel::displayData(uint row, uint col) const {
  switch (col) {
  case COL_ID:
    return str::number(row + 1);
  case COL_TYPE:
    return calc::Reflection::typeTag(hub_.reflections().at(row)->type());
  default:
    NEVER return EMPTY_STR;
  }
}

str ReflectionsModel::displayData(uint row) const {
  return displayData(row, COL_ID) + ": " + displayData(row, COL_TYPE);
}

QVariant ReflectionsModel::data(rcIndex index, int role) const {
  int row = index.row();
  if (row < 0 || rowCount() <= row)
    return EMPTY_VAR;

  switch (role) {
  case Qt::DisplayRole: {
    int col = index.column();
    if (col < DCOL)
      return EMPTY_VAR;

    switch (col) {
    case COL_ID:
    case COL_TYPE:
      return displayData(to_u(row), to_u(col));
    default:
      return EMPTY_VAR;
    }
  }

  case GetDatasetRole:
    return QVariant::fromValue<calc::shp_Reflection>(hub_.reflections().at(to_u(row)));
  default:
    return EMPTY_VAR;
  }
}

QVariant ReflectionsModel::headerData(int col, Qt::Orientation,
                                      int role) const {
  if (Qt::DisplayRole == role && COL_ID == col)
    return "#";
  else
    return EMPTY_VAR;
}

void ReflectionsModel::addReflection(fit::ePeakType type) {
  hub_.addReflection(type);
}

void ReflectionsModel::remReflection(uint i) {
  hub_.remReflection(i);
}

str_lst ReflectionsModel::names() const {
  str_lst ns;
  for_i (rowCount())
    ns.append(displayData(to_u(i)));
  return ns;
}

//------------------------------------------------------------------------------
}
// eof
