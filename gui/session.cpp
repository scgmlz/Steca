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

  setImageCut(x1,y1,x2,y2);
}

QByteArray Session::save() const {
  QByteArray json;

  QJsonArray files;

  for (auto file: coreSession->getDataFiles()) {
    files.append(file->getInfo().absoluteFilePath());
  }

  QJsonObject top;
  top["files"] = files;
  top["corr.file"] = coreSession->getCorrFile().getInfo().absoluteFilePath();

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
    setImageCut(imagecut_t());

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

void Session::setImageCut(imagecut_t const& imageCut_) {
  auto size = coreSession->getImageSize();
  if (size.isEmpty())
    imageCut = imagecut_t();
  else {
    imageCut = imageCut_;
    // make sure that cut is valid
    imageCut.top    = qMin(imageCut.top,    qMax(size.height() - imageCut.bottom - 1, 0));
    imageCut.bottom = qMin(imageCut.bottom, qMax(size.height() - imageCut.top    - 1, 0));
    imageCut.left   = qMin(imageCut.left,   qMax(size.width()  - imageCut.right  - 1, 0));
    imageCut.right  = qMin(imageCut.right,  qMax(size.width()  - imageCut.left   - 1, 0));
  }

  emit imageCutChanged();
}

void Session::setImageCut(int top, int bottom, int left, int right) {
  setImageCut(imagecut_t(top,bottom,left,right));
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
