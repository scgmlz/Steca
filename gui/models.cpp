#include "models.h"
#include "session.h"

namespace model {
//------------------------------------------------------------------------------

SessionModel::SessionModel(Session& session_): session(session_) {
}

uint SessionModel::numFiles(bool withCorr) {
  return session.numFiles(withCorr);
}

void SessionModel::remFile(uint i) {
  session.remFile(i);
}

void SessionModel::setSelectedFile(core::shp_File file) {
  session.setSelectedFile(file);
}

void SessionModel::setSelectedDataset(core::shp_Dataset dataset) {
  session.setSelectedDataset(dataset);
}

//------------------------------------------------------------------------------

FileViewModel::FileViewModel(Session& session): SessionModel(session) {
}

int FileViewModel::rowCount(QModelIndex const&) const {
  return session.numFiles(true);
}

QVariant FileViewModel::data(QModelIndex const& index,int role) const {
  auto row = index.row(), cnt = rowCount(index);
  if (row < 0 || row >= cnt) return QVariant();

  bool isCorrectionFile = session.hasCorrFile() && row+1 == cnt;

  switch (role) {
    case IsCorrectionFileRole:
      return isCorrectionFile;
    case Qt::DisplayRole: {
      str s = session.getFile(row)->getName();
      static str Corr("Corr: ");
      if (isCorrectionFile) s = Corr + s;
      return s;
    }
    case GetFileRole:
      return QVariant::fromValue<core::shp_File>(session.getFile(row));
    default:
      return QVariant();
  }
}

void FileViewModel::signalReset() {
  beginResetModel();
  endResetModel();    // emits a signal to connected views
}

//------------------------------------------------------------------------------

DatasetViewModel::DatasetViewModel(Session& session)
: SessionModel(session), coreFile(nullptr), infoItems(nullptr) {
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

QVariant DatasetViewModel::headerData(int section, Qt::Orientation orientation, int role) const {
  if (Qt::Horizontal != orientation || Qt::DisplayRole != role
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

void DatasetViewModel::setInfoItems(panel::InfoItems const* infoItems_) {
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
}
// eof
