#include "thehub.h"

#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QApplication>

//------------------------------------------------------------------------------

// TODO handle long operations asynchronously
class WaitCursor {
public:
  WaitCursor() {
    QApplication::setOverrideCursor(Qt::WaitCursor);
  }

 ~WaitCursor() {
    QApplication::restoreOverrideCursor();
  }
};

//------------------------------------------------------------------------------

Action::Action(rcstr text, rcstr tip, rcstr iconFile, QObject* parent): super(text,parent) {
  setToolTip(tip);

  if (!iconFile.isEmpty())
    setIcon(QIcon(iconFile));
}

//------------------------------------------------------------------------------

PushAction::PushAction(rcstr text, rcstr tip, rcstr iconFile, QObject* parent)
: super(text,tip,iconFile,parent) {
}

//------------------------------------------------------------------------------

ToggleAction::ToggleAction(rcstr text, rcstr tip, rcstr iconFile, QObject* parent)
: super(text,tip,iconFile,parent) {
  setCheckable(true);
}

//------------------------------------------------------------------------------

Settings::Settings(rcstr group) {
  setFallbacksEnabled(false);
  beginGroup(group);
}

Settings::~Settings() {
  endGroup();
}

QVariant Settings::readVariant(rcstr key, const QVariant &def) {
  auto val = value(key,def);
  return val;
}

void Settings::saveVariant(rcstr key, const QVariant &val) {
  setValue(key,val);
}

void Settings::read(rcstr key, QAction* act, bool def) {
  ASSERT(act->isCheckable())
  if (act) act->setChecked(readVariant(key,def).toBool());
}

void Settings::save(rcstr key, QAction* act) {
  ASSERT(act->isCheckable())
  if (act) saveVariant(key,act->isChecked());
}

void Settings::read(rcstr key, QSpinBox* box, int def) {
  if (box) box->setValue(readVariant(key,def).toInt());
}

void Settings::save(rcstr key, QSpinBox* box) {
  if (box) saveVariant(key,box->value());
}

void Settings::read(rcstr key, QDoubleSpinBox* box, qreal def) {
  if (box) box->setValue(readVariant(key,def).toDouble());
}

void Settings::save(rcstr key, QDoubleSpinBox* box) {
  if (box) saveVariant(key,box->value());
}

//------------------------------------------------------------------------------

TheHub::TheHub(): session(new core::Session())
, globalNorm(false)
, fileViewModel(*this), datasetViewModel(*this), reflectionViewModel(*this) {
  initActions();
  configActions();
}

TheHub::~TheHub() {
  delete session;
}

#define PUSH_ACTION(act,text,tip,icon) \
  lastAction = act = new PushAction(text,tip,icon,this);
#define TOGL_ACTION(act,text,tip,icon) \
  lastAction = act  = new ToggleAction(text,tip,icon,this);
#define ACTION_KEY(key) \
  lastAction->setShortcut(key);

void TheHub::initActions() {
  typedef QKeySequence QKey; QAction *lastAction;

  PUSH_ACTION(actAddFiles,    "Add files...","Add files",":/icon/add")                        ACTION_KEY(Qt::CTRL|Qt::Key_O)
  PUSH_ACTION(actRemoveFile,  "Remove selected file","Remove selected file",":/icon/rem")     ACTION_KEY(QKey::Delete);

  PUSH_ACTION(actLoadCorrectionFile,"Load correction file...", "Load correction file", "")    ACTION_KEY(Qt::SHIFT|Qt::CTRL|Qt::Key_O);
  TOGL_ACTION(actImagesEnableCorr,  "Enable correction file",  "Enable correction by correction file", ":/icon/useCorrection")

  PUSH_ACTION(actLoadSession,"Load session...","","")
  PUSH_ACTION(actSaveSession,"Save session...","","")

  PUSH_ACTION(actExportDiffractogramCurrent,"Current only...","","")
  PUSH_ACTION(actExportDiffractogramAllSeparateFiles,"All to separate files...","","")
  PUSH_ACTION(actExportDiffractogramSingleFile,"All to a single file...","","")

  PUSH_ACTION(actExportImagesWithMargins,"With margins...","","")
  PUSH_ACTION(actExportImagesWithoutMargins,"Without margins...","","")

  PUSH_ACTION(actQuit ,"Quit", "",  "") ACTION_KEY(QKey::Quit)

  PUSH_ACTION(actUndo ,"Undo",  "", "") ACTION_KEY(QKey::Undo)
  PUSH_ACTION(actRedo ,"Redo",  "", "") ACTION_KEY(QKey::Redo)
  PUSH_ACTION(actCut  ,"Cut",   "", "") ACTION_KEY(QKey::Cut)
  PUSH_ACTION(actCopy ,"Copy",  "", "") ACTION_KEY(QKey::Copy)
  PUSH_ACTION(actPaste,"Paste", "", "") ACTION_KEY(QKey::Paste)

  TOGL_ACTION(actViewStatusbar ,"Statusbar",  "", "") ACTION_KEY(Qt::Key_F12)
#ifndef Q_OS_OSX
  TOGL_ACTION(actFullscreen    ,"Fullscreen", "", "") ACTION_KEY(Qt::Key_F11)
#endif
  PUSH_ACTION(actViewReset    ,"Reset", "","")

  PUSH_ACTION(actPreferences         ,"Preferences...","","")
  PUSH_ACTION(actFitErrorParameters  ,"Fit error parameters...","","")

  PUSH_ACTION(actAbout     ,"About...","","")

  TOGL_ACTION(actSelectPeak           ,"Select Peak", "Select Peak", ":/icon/selectPeak")
  PUSH_ACTION(actReflectionPeak      ,"Peak",  "Set reflection peak",  ":/icon/selectHight")
  PUSH_ACTION(actReflectionWidth     ,"Width", "Set reflection width", ":/icon/selectWidth")
  PUSH_ACTION(actReflectionAdd       ,"Add",   "Add reflection",       ":/icon/add")
  PUSH_ACTION(actReflectionRemove    ,"Remove","Remove reflection",    ":/icon/rem")
  PUSH_ACTION(actReflectionSelectRegion,"Select reflection fit region","","")

  PUSH_ACTION(actCalculatePolefigures,"Calculate polefigures...","","") // TODO polefigure or pole figure?
  PUSH_ACTION(actCalculateHistograms ,"Calculate histograms..","","")

  TOGL_ACTION(actImagesLink           ,"Link",         "Use the same value for all cuts", ":/icon/linkNew")
  TOGL_ACTION(actImageOverlay         ,"overlay",      "Show cut", ":/icon/imageCrop")
  TOGL_ACTION(actImagesGlobalNorm     ,"global norm.", "Display data using a fixed intensity scale", ":/icon/scale")
  PUSH_ACTION(actImageRotate         ,"Rotate",       "Rotate 90° clockwise", ":/icon/rotate0") ACTION_KEY(Qt::CTRL|Qt::Key_R)
  TOGL_ACTION(actImageMirror          ,"Mirror",       "Mirror image", ":/icon/mirror_horz")

  PUSH_ACTION(actBackgroundClear     ,"Clear background fit regions",    "Clear regions for background fitting", ":/icon/clearBackground")
  TOGL_ACTION(actBackgroundBackground ,"Select background fit regions",    "Select regions for background fitting", ":/icon/pekBackground")
  TOGL_ACTION(actBackgroundShowFit    ,"BackgroundEye", "Show background fit", ":/icon/showBackground")

  TOGL_ACTION(actHasBeamOffset        ,"Beam centre offset", "Enable beam center offset (for X-ray instruments)", ":/icon/beam")

  PUSH_ACTION(actNormalizationDisable    ,"Disable","","")
  PUSH_ACTION(actNormalizationMeasureTime,"Measurement Time","","")
  PUSH_ACTION(actNormalizationMonitor    ,"Monitor counts","","")
      PUSH_ACTION(actNormalizationBackground ,"Background level","","")
}

void TheHub::configActions() {
  actRemoveFile->setEnabled(false);
  actImagesEnableCorr->setEnabled(false);
  actReflectionRemove->setEnabled(false);

  connect(this, &thisCls::correctionEnabled, [this](bool on) {
    actImagesEnableCorr->setChecked(on);
    actImagesEnableCorr->setEnabled(hasCorrFile());
  });

  connect(this, &thisCls::fileSelected, this, [this](core::shp_File file) {
    actRemoveFile->setEnabled(!file.isNull());
  });

  connect(actImagesEnableCorr, &QAction::toggled, [this](bool on) {
    enableCorrection(on);
  });

  connect(actImagesGlobalNorm, &QAction::toggled, [this](bool on) {
    globalNorm = on;
    emit displayChange();
  });

  connect(actImageMirror, &QAction::toggled, [this](bool on) {
    setImageMirror(on);
  });

  connect(actImageRotate, &QAction::triggered, [this]() {
    setImageRotate(session->getImageTransform().nextRotate());
  });
}

uint TheHub::numFiles(bool withCorr) const {
  return session->numFiles(withCorr);
}

bool TheHub::hasCorrFile() const {
  return session->hasCorrFile();
}

bool TheHub::isCorrFile(uint index) const {
  return hasCorrFile() && numFiles(true) == index+1;
}

str TheHub::fileName(uint index) const {
  return getFile(index)->getName();
}

str TheHub::filePath(uint index) const {
  return getFile(index)->getInfo().absoluteFilePath();
}

core::shp_File TheHub::getFile(uint index) const {
  return session->getFile(index);
}

void TheHub::remFile(uint i) {
  if (hasCorrFile() && numFiles(true) == i+1) {
    session->remCorrFile();
    emit correctionEnabled(session->isCorrEnabled());
    emit filesChanged();
  } else {
    session->remFile(i);
    emit filesChanged();
  }

  if (0==numFiles(true)) {
    setSelectedDataset(core::shp_Dataset());
    setImageCut(true,false,core::ImageCut());
  }
}

void TheHub::setSelectedFile(core::shp_File file) {
  emit fileSelected(file);
}

void TheHub::setSelectedDataset(core::shp_Dataset dataset) {
  emit datasetSelected(dataset);
}

void TheHub::load(QFileInfo const& fileInfo) THROWS {
  QFile file(fileInfo.absoluteFilePath());
  RUNTIME_CHECK(file.open(QIODevice::ReadOnly), "File cannot be opened");
  load(file.readAll());
}

// TODO merge load / save, DRY
void TheHub::load(QByteArray const& json) THROWS {
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

  setGeometry(
    det["distance"].toDouble(),
    det["pixel_size"].toDouble(),
    det["hasbeamoffset"].toBool(),
    QPoint(det["offset_x"].toDouble(),det["offset_y"].toDouble()));

  setImageRotate(core::ImageTransform(top["transform"].toInt()));
}

QByteArray TheHub::save() const {
  auto const &g = session->getGeometry();
  QJsonObject det {
    { "distance",     g.sampleDetectorSpan  },
    { "pixel_size",   g.pixSpan             },
    { "isbeamoffset", g.hasBeamOffset       },
    { "offset_x",     g.middlePixOffset.x() },
    { "offset_y",     g.middlePixOffset.y() },
  };

  QJsonArray files;
  for_i (numFiles(false)) {
    files.append(getFile(i)->getInfo().absoluteFilePath());
  }

  auto const &ic = session->getImageCut();
  QJsonObject cut {
    { "top",    (int)ic.top     },
    { "bottom", (int)ic.bottom  },
    { "left",   (int)ic.left    },
    { "right",  (int)ic.right   },
  };

  QJsonObject top {
    { "detector",   det                 },
    { "cut",        cut                 },
    { "transform",  session->getImageTransform().val  },
    { "files",      files               },
    { "corr.file",  hasCorrFile() ? session->getCorrFile()->getInfo().absoluteFilePath() : "" },
  };

  return QJsonDocument(top).toJson();
}

void TheHub::addFile(rcstr filePath) THROWS {
  WaitCursor __;

  core::shp_File file = session->addFile(filePath);
  if (!file.isNull()) {
    emit filesChanged();
  }
}

void TheHub::addFiles(str_lst filePaths) THROWS {
  WaitCursor __;

  for (auto &filePath: filePaths)
    addFile(filePath);
}

void TheHub::loadCorrFile(rcstr filePath) {
  auto file = session->loadCorrFile(filePath);
  emit correctionEnabled(session->isCorrEnabled());
  emit filesChanged();
}

void TheHub::enableCorrection(bool on) {
  session->enableCorrection(on);
  emit correctionEnabled(session->isCorrEnabled());
}

const core::ImageCut &TheHub::getImageCut() const {
  return session->getImageCut();
}

void TheHub::setImageCut(bool topLeft, bool linked, core::ImageCut const& imageCut) {
  session->setImageCut(topLeft,linked,imageCut);
  session->calcIntensCorrArray();
  emit geometryChanged();
}

QSize TheHub::getImageSize() const {
  return session->getImageSize();
}

uint TheHub::pixIndexNoTransform(uint x, uint y) const {
  return session->pixIndexNoTransform(x,y);
}

core::intens_t TheHub::pixIntensity(core::Image const& image, uint x, uint y) const {
  return session->pixIntensity(image, x, y);
}

core::AngleCorrArray const& TheHub::calcAngleCorrArray(qreal tthMitte) {
  return session->calcAngleCorrArray(tthMitte);
}

core::Borders const& TheHub::getCut() const {
  return session->getCut();
}

const core::Geometry &TheHub::getGeometry() const {
  return session->getGeometry();
}

void TheHub::setGeometry(qreal sampleDetectorSpan, qreal pixSpan, bool hasBeamOffset, QPoint const& middlePixOffset) {
  session->setGeometry(sampleDetectorSpan,pixSpan,hasBeamOffset,middlePixOffset);
  emit geometryChanged();
}

void TheHub::setBackgroundPolynomialDegree(uint degree) {
  emit backgroundPolynomialDegree(degree);
}

void TheHub::doReadSettings() {
  emit readSettings();
}

void TheHub::doSaveSettings() {
  emit saveSettings();
}

void TheHub::setImageRotate(core::ImageTransform rot) {
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
  session->setImageRotate(rot);
  setImageCut(true,false,session->getImageCut()); // TODO make makeSafeCut()
  emit geometryChanged();
}

void TheHub::setImageMirror(bool on) {
  actImageMirror->setChecked(on);
  session->setImageMirror(on);
  emit geometryChanged();
}

//------------------------------------------------------------------------------
// eof
