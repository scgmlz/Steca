#include "session.h"
#include "mainwin.h"
#include <QAction>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QApplication>

class WaitCursor {
public:
  WaitCursor() {
    QApplication::setOverrideCursor(Qt::WaitCursor);
  }

 ~WaitCursor() {
    QApplication::restoreOverrideCursor();
  }
};

Session::Session()
: fileViewModel(*this), datasetViewModel(*this) {
}

Session::~Session(){
}

void Session::load(QFileInfo const& fileInfo) THROWS {
  QFile file(fileInfo.absoluteFilePath());
  RUNTIME_CHECK(file.open(QIODevice::ReadOnly), "File cannot be opened");
  load(file.readAll());
}

// TODO merge load / save, DRY
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

  loadCorrFile(top["corr.file"].toString());

  auto cut = top["cut"].toObject();
  int y1 = qMax(0,cut["top"].toInt());
  int y2 = qMax(0,cut["bottom"].toInt());
  int x1 = qMax(0,cut["left"].toInt());
  int x2 = qMax(0,cut["right"].toInt());

  setImageCut(true,false,core::ImageCut(x1,y1,x2,y2));

  auto det = top["detector"].toObject();

  sampleDetectorSpan = det["distance"].toDouble();
  pixSpan            = det["pixel_size"].toDouble();
  hasBeamOffset      = det["hasbeamoffset"].toBool();
  middlePixXOffset   = det["offset_x"].toDouble();
  middlePixYOffset   = det["offset_y"].toDouble();

  setImageRotate(core::Image::Transform(top["transform"].toInt()));

  emit sessionLoaded();
}

// TODO mainwin passed to access transformation actions - rethink
QByteArray Session::save() const {
  QByteArray json;

  QJsonObject top;

  QJsonObject det;

  det["distance"]     = sampleDetectorSpan;
  det["pixel_size"]   = pixSpan;
  det["isbeamoffset"] = hasBeamOffset;
  det["offset_x"]     = middlePixXOffset;
  det["offset_y"]     = middlePixYOffset;

  top["detector"] = det;

  QJsonArray files;

  for (auto file: dataFiles) {
    files.append(file->getInfo().absoluteFilePath());
  }

  top["files"] = files;
  if (!corrFile.isNull())
    top["corr.file"] = corrFile->getInfo().absoluteFilePath();

  QJsonObject cut;

  cut["top"]    = (int)imageCut.top;
  cut["bottom"] = (int)imageCut.bottom;
  cut["left"]   = (int)imageCut.left;
  cut["right"]  = (int)imageCut.right;

  top["cut"] = cut;
  top["transform"] = imageTransform.val;

  QJsonDocument doc(top);
  return doc.toJson();
}

void Session::addFile(rcstr filePath) THROWS {
  WaitCursor __;

  super::addFile(filePath);
  emit filesChanged();
}

void Session::addFiles(str_lst filePaths) THROWS {
  WaitCursor __;

  for (auto &filePath: filePaths)
    super::addFile(filePath);
  emit filesChanged();
}

void Session::remFile(uint i) {
  super::remFile(i);

  setSelectedFile(core::shp_File());
  emit filesChanged();

  if (0==numFiles(true))
    setImageCut(true,false,core::ImageCut());
}

void Session::loadCorrFile(rcstr filePath) {
  super::loadCorrFile(filePath);
  emit filesChanged();
}

void Session::setSelectedFile(core::shp_File file) {
  setSelectedDataset(core::shp_Dataset());
  emit fileSelected(file);
}

void Session::setSelectedDataset(core::shp_Dataset dataset) {
  emit datasetSelected(dataset);
}

void Session::setImageCut(bool topLeft, bool linked, core::ImageCut const& imageCut) {
  super::setImageCut(topLeft,linked,imageCut);
  calcIntensCorrArray();
  emit imageCutChanged();
}

void Session::setImageMirror(bool on) {
  actImageMirror->setChecked(on);
  super::setImageMirror(on);
}

void Session::setImageRotate(core::Image::Transform rot) {
  pcstr rotateIconFile, mirrorIconFile;
  switch (rot.val & 3) {
  case 0:
    rotateIconFile = ":/icon/rotate0";
    mirrorIconFile = ":/icon/mirror_horz";
    break;
  case 1:
    rotateIconFile = ":/icon/rotate1";
    mirrorIconFile = ":/icon/mirror_vert";
    break;
  case 2:
    rotateIconFile = ":/icon/rotate2";
    mirrorIconFile = ":/icon/mirror_horz";
    break;
  case 3:
    rotateIconFile = ":/icon/rotate3";
    mirrorIconFile = ":/icon/mirror_vert";
    break;
  }

  actImageRotate->setIcon(QIcon(rotateIconFile));
  actImageMirror->setIcon(QIcon(mirrorIconFile));
  super::setImageRotate(rot);
}

void Session::nextImageRotate() {
  setImageRotate(imageTransform.nextRotate());
}

// eof
