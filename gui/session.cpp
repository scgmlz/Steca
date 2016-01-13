#include "session.h"
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>

Session::Session(): coreSession(new core::Session)
, selectedFile(nullptr)
, fileListModel(*this), datasetListModel(*this) {
}

Session::~Session(){
}

void Session::load(QFileInfo const& fileInfo) THROWS {
  QFile file(fileInfo.absoluteFilePath());
  RUNTIME_CHECK(file.open(QIODevice::ReadOnly), "File cannot be opened");
  load(file.readAll());
}

void Session::load(QByteArray const& json) THROWS {
  QJsonParseError parseError;
  QJsonDocument doc(QJsonDocument::fromJson(json,&parseError));
  RUNTIME_CHECK(QJsonParseError::NoError==parseError.error, "Error parsing file");

  WaitCursor __;

  while (numFiles(true)>0) remFile(0);

  auto top   = doc.object();
  auto files = top["files"].toArray();

  for (auto file: files) {
    addFile(file.toString());
  }

  setCorrFile(top["corr.file"].toString());

  auto cut = top["cut"].toObject();
  int y1 = cut["top"].toInt();
  int y2 = cut["bottom"].toInt();
  int x1 = cut["left"].toInt();
  int x2 = cut["right"].toInt();

  setImageCut(true,false,x1,y1,x2,y2);
}

QByteArray Session::save() const {
  QByteArray json;

  QJsonArray files;

  for (auto file: coreSession->getDataFiles()) {
    files.append(file->getInfo().absoluteFilePath());
  }

  QJsonObject top;
  top["files"] = files;
  auto corrFile = coreSession->getCorrFile();
  if (!corrFile.name().isEmpty()) top["corr.file"] = corrFile.getInfo().absoluteFilePath();

  QJsonObject cut;

  cut["top"]    = imageCut.top;
  cut["bottom"] = imageCut.bottom;
  cut["left"]   = imageCut.left;
  cut["right"]  = imageCut.right;

  top["cut"] = cut;

  QJsonDocument doc(top);
  return doc.toJson();
}

void Session::addFile(rcstr filePath) THROWS {
  WaitCursor __;

  coreSession->addFile(filePath);
  emit filesChanged();
}

void Session::addFiles(str_lst filePaths) THROWS {
  WaitCursor __;

  for (auto& filePath: filePaths)
    coreSession->addFile(filePath);
  emit filesChanged();
}

void Session::remFile(uint i) {
  auto &files = coreSession->getDataFiles();
  if ((uint)files.count() == i)
    coreSession->setCorrFile(str::null);
  else
    coreSession->remFile(i);

  if (0==numFiles(true))
    setImageCut(true,false,imagecut_t());

  emit filesChanged();
}

uint Session::numFiles(bool withCorr) {
  return coreSession->getDataFiles().count()
      + (withCorr && hasCorrFile() ? 1 : 0);
}

core::File const& Session::getFile(uint i) {
  auto &files = coreSession->getDataFiles();
  return ((uint)files.count() == i)
    ? coreSession->getCorrFile()
    : *files.at(i);
}

str Session::fileName(uint i) {
  return getFile(i).name();
}

bool Session::hasCorrFile() {
  return coreSession->hasCorrFile();
}

void Session::setCorrFile(rcstr filePath) {
  coreSession->setCorrFile(filePath);
  emit filesChanged();
}

void Session::setSelectedFile(pcCoreFile file) {
  setSelectedDataset(nullptr);
  emit fileSelected((selectedFile = file));
}

void Session::setSelectedDataset(pcCoreDataset dataset) {
  emit datasetSelected((selectedDataset = dataset));
}

core::Dataset const&Session::getDataset(uint i) {
  pcCoreFile file = selectedFile;
  ASSERT(file)
      return *(file->getDatasets().at(i));
}

Session::imagecut_t::imagecut_t(int top_, int bottom_, int left_, int right_)
: top(top_), bottom(bottom_), left(left_), right(right_) {
}

const Session::imagecut_t &Session::getImageCut() const {
  return imageCut;
}

void Session::setImageCut(bool topLeft, bool linked, imagecut_t const& imageCut_) {
  auto size = coreSession->getImageSize();
  if (size.isEmpty())
    imageCut = imagecut_t();
  else {
    auto limit = [linked](int &thisOne, int &thatOne, int maxTogether) {
      if (linked && thisOne+thatOne>=maxTogether) {
        thisOne = thatOne = qMax(0, (maxTogether-1) / 2);
      } else {
        thisOne = qMax(qMin(thisOne, maxTogether - thatOne - 1), 0);
        thatOne = qMax(qMin(thatOne, maxTogether - thisOne - 1), 0);
      }
    };

    imageCut = imageCut_;
    // make sure that cut values are valid; in the right order
    if (topLeft) {
      limit(imageCut.top,   imageCut.bottom,  size.height());
      limit(imageCut.left,  imageCut.right,   size.width());
    } else {
      limit(imageCut.bottom,imageCut.top,     size.height());
      limit(imageCut.right, imageCut.left,    size.width());
    }
  }

  emit imageCutChanged();
}

void Session::setImageCut(bool topLeft, bool linked, int top, int bottom, int left, int right) {
  setImageCut(topLeft,linked,imagecut_t(top,bottom,left,right));
}

//-----------------------------------------------------------------------------

Session::FileListModel::FileListModel(Session& session_): session(session_) {
}

int Session::FileListModel::rowCount(QModelIndex const&) const {
  return session.numFiles(true);
}

QVariant Session::FileListModel::data(QModelIndex const& index,int role) const {
  auto row = index.row(), cnt = rowCount(index);
  if (row < 0 || row >= cnt) return QVariant();

  bool isCorrectionFile = session.hasCorrFile() && row+1 == cnt;

  switch (role) {
    case IsCorrectionFileRole:
      return isCorrectionFile;
    case Qt::DisplayRole: {
      str s = session.fileName(row);
      static str Corr("Corr: ");
      if (isCorrectionFile) s = Corr + s;
      return s;
    }
    case GetFileRole:
      return QVariant::fromValue<pcCoreFile>(&(session.getFile(row)));
    default:
      return QVariant();
  }
}

//-----------------------------------------------------------------------------

Session::DatasetListModel::DatasetListModel(Session& session_): session(session_) {
}

int Session::DatasetListModel::rowCount(QModelIndex const&) const {
  pcCoreFile file = session.selectedFile;
  return file ? file->getDatasets().count() : 0;
}

QVariant Session::DatasetListModel::data(QModelIndex const& index,int role) const {
  pcCoreFile file = session.selectedFile;
  if (!file) return QVariant();

  auto row = index.row(), cnt = rowCount(index);
  if (row < 0 || row >= cnt) return QVariant();

  switch (role) {
    case Qt::DisplayRole: {
      str s = str("%1 ").arg(row) % file->getDatasets().at(row)->getComment();
      return s;
    }
    case GetFileRole:
      return QVariant::fromValue<pcCoreDataset>(&(session.getDataset(row)));
    default:
      return QVariant();
  }
}

// eof
