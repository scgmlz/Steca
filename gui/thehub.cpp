// ************************************************************************** //
//
//  STeCa2:    StressTexCalculator ver. 2
//
//! @file      thehub.cpp
//!
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016
//! @authors   Scientific Computing Group at MLZ Garching
//! @authors   Original version: Christian Randau
//! @authors   Version 2: Antti Soininen, Jan Burle, Rebecca Brydon
//
// ************************************************************************** //

#include "thehub.h"
#include "mainwin.h"
#include "types/core_json.h"
#include "core_reflection.h"
#include "io/core_io.h"

#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QJsonDocument>
#include <QApplication>
#include <QActionEvent>
#include <QDir>

namespace gui {
//------------------------------------------------------------------------------

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
, fixedIntensityScale(false)
, fileViewModel(*this), datasetViewModel(*this), reflectionViewModel(*this) {
  initActions();
  configActions();
}

#define TRIG(act,text,tip) (*(Action*)(actions.act = new TriggerAction(text,tip,this)))
#define TOGL(act,text,tip) (*(Action*)(actions.act = new ToggleAction(text,tip,this)))

void TheHub::initActions() { // REVIEW all action texts and tips and icons
  using QKey = QKeySequence;

  TRIG(addFiles, "Add files", "")
      .icon(":/icon/add").key(Qt::CTRL|Qt::Key_O)
      .dialog();
  TRIG(remFile, "Remove file", "Remove selected file")
      .icon(":/icon/rem").key(QKey::Delete);

  TOGL(enableCorr, "Enable correction file", "Enable correction by correction file")
      .alt("Disable correction file", "Disable correction by correction file")
      .icon(":/icon/useCorrection").key(Qt::SHIFT|Qt::CTRL|Qt::Key_C).dialog();
  TRIG(remCorr, "Remove correction file", "")
      .icon(":/icon/clear").key(QKey::Delete);

  TRIG(loadSession, "Load session", "")
      .dialog();
  TRIG(saveSession, "Save session", "")
      .dialog();

  TRIG(quit, "Quit", "")
      .key(QKey::Quit);

  TOGL(viewStatusbar, "Statusbar",  "")
      .key(Qt::Key_F12);
#ifndef Q_OS_OSX
  TOGL(fullScreen, "FullScreen", "")
      .key(Qt::Key_F11);
#endif
  TOGL(viewDockFiles,       "Files", "")
      .key(Qt::Key_F8);
  TOGL(viewDockDatasets,    "Datasets", "")
      .key(Qt::Key_F9);
  TOGL(viewDockDatasetInfo, "Dataset info", "")
      .key(Qt::Key_F10);
  TRIG(viewReset, "Reset", "");

  TRIG(about, "About", "")
      .dialog();

  TRIG(addReflection, "Add", "Add reflection")
      .icon(":/icon/add");
  TRIG(remReflection, "Remove", "Remove reflection")
       .icon(":/icon/rem");

  TRIG(outputPolefigures, "Pole figures", "")
      .dialog();
  TRIG(outputHistograms, "Histograms", "")
      .dialog();

  TOGL(linkCuts, "Link", "Use the same value for all cuts")
      .alt("Unlink", "Use different value for cuts")
      .icon(":/icon/linkNew");
  TOGL(showCut, "overlay", "Show cut")
      .alt("overlay", "Hide cut")
      .icon(":/icon/imageCrop");
  TOGL(fixedIntensityDisplay, "fixed Intensity", "Display data using a fixed intensity scale")
      .alt("fixed Intensity", "Display data using non-fixed intensity scale")
      .icon(":/icon/scale");
  TRIG(rotateImage, "Rotate", "Rotate 90° clockwise")
      .icon(":/icon/rotate0").key(Qt::CTRL|Qt::Key_R);
  TOGL(mirrorImage, "Mirror", "Mirror image")
      .icon(":/icon/mirror_horz");

  TOGL(fitTool, "Select background fit regions", "Select regions for background fitting")
      .icon(":/icon/peakBackground");
  TRIG(fitBgClear, "Clear background fit regions", "Clear regions for background fitting")
      .icon(":/icon/clear");
  TOGL(fitShow, "BackgroundEye", "Show background fit")
      .alt("BackgroundEye", "Hide background fit")
      .icon(":/icon/showBackground");

  TOGL(hasBeamOffset, "Beam centre offset", "Enable beam center offset (for X-ray instruments)")
      .alt("Beam centre offset", "Disable beam center offset")
      .icon(":/icon/beam");
}

void TheHub::configActions() {
  actions.remFile->setEnabled(false);
  actions.remReflection->setEnabled(false);

  connect(this, &thisClass::corrEnabled, [this](bool on) {
    actions.enableCorr->setChecked(on);
  });

  connect(this, &thisClass::filesSelected, this, [this]() {
    actions.remFile->setEnabled(!collectedFromFiles().isEmpty());
  });

  connect(actions.enableCorr, &QAction::toggled, [this](bool on) {
    enableCorrection(on);
  });

  connect(actions.remCorr, &QAction::triggered, [this]() {
    setCorrFile(EMPTY_STR);
  });

  connect(actions.fixedIntensityDisplay, &QAction::toggled, [this](bool on) {
    fixedIntensityScale = on;
    emit displayChange();
  });

  connect(actions.mirrorImage, &QAction::toggled, [this](bool on) {
    setImageMirror(on);
  });

  connect(actions.rotateImage, &QAction::triggered, [this]() {
    setImageRotate(session->imageTransform().nextRotate());
  });
}

uint TheHub::numFiles() const {
  return session->numFiles();
}

str TheHub::fileName(uint index) const {
  return getFile(index)->fileName();
}

str TheHub::filePath(uint index) const {
  return getFile(index)->fileInfo().absoluteFilePath();
}

core::shp_File TheHub::getFile(uint index) const {
  return session->file(index);
}

void TheHub::remFile(uint i) {
  session->remFile(i);
  emit filesChanged();

  if (0==numFiles()) {
    tellSelectedDataset(core::shp_Dataset()); // REVIEW out?
    setImageCut(true, false, core::ImageCut());
  }
}

bool TheHub::hasCorrFile() const {
  return session->hasCorrFile();
}

core::rcImage TheHub::corrImage() const {
  return session->corrImage();
}

void TheHub::tellFilesSelectedDatasetsChanged() {
  emit filesSelected();
  emit datasetsChanged();
  uint cnt = collectedDatasets().count();
  if (!cnt) tellSelectedDataset(core::shp_Dataset());
}

void TheHub::tellSelectedDataset(core::shp_Dataset dataset) {
  emit datasetSelected(dataset);
}

void TheHub::tellSelectedReflection(core::shp_Reflection reflection) {
  emit reflectionSelected((selectedReflection = reflection));
}

void TheHub::tellReflectionData(core::shp_Reflection reflection) {
  emit reflectionData(reflection);
}

void TheHub::tellReflectionData(core::rcRange range, core::rcXY peak, qreal fwhm, bool withGuesses) {
  emit reflectionValues(range, peak, fwhm, withGuesses);
}

core::shp_ImageLens TheHub::lensNoCut(core::rcImage image) const {
  return session->lens(image, true, false);
}

core::shp_Lens TheHub::lens(core::rcDataset dataset) const {
  return session->lens(dataset, true, true, session->norm());
}

core::shp_Lens TheHub::lensNoCut(core::rcDataset dataset) const {
  return session->lens(dataset, true, false, session->norm());
}

core::AngleMap const& TheHub::angleMap(core::rcDataset dataset) const {
  return session->angleMap(dataset);
}

static str const KEY_FILES("files");
static str const KEY_SELECTED_FILES("selected files");
static str const KEY_CORR_FILE("correction file");
static str const KEY_CUT("cut");
static str const KEY_TOP("top");
static str const KEY_BOTTOM("bottom");
static str const KEY_LEFT("left");
static str const KEY_RIGHT("right");
static str const KEY_DETECTOR("detector");
static str const KEY_DISTANCE("distance");
static str const KEY_PIX_SIZE("pixel size");
static str const KEY_OFFSET_BEAM("offset beam");
static str const KEY_BEAM_OFFSET("beam offset");
static str const KEY_TRANSFORM("image transform");
static str const KEY_BG_DEGREE("background degree");
static str const KEY_BG_RANGES("background ranges");
static str const KEY_REFLECTIONS("reflections");

void TheHub::saveSession(QFileInfo const& fileInfo) const {
  QFile file(fileInfo.filePath());
  RUNTIME_CHECK(file.open(QIODevice::WriteOnly), "File cannot be opened");

  QDir::setCurrent(fileInfo.absolutePath());
  auto written = file.write(saveSession());
  RUNTIME_CHECK(written >= 0, "Could not write session");
}

QByteArray TheHub::saveSession() const {
  using core::JsonObj;
  using core::JsonArr;

  JsonObj top;

  auto &geo = session->geometry();
  top.saveObj(KEY_DETECTOR, JsonObj()
    .saveReal(KEY_DISTANCE,    geo.detectorDistance)
    .saveReal(KEY_PIX_SIZE,    geo.pixSize)
    .saveBool(KEY_OFFSET_BEAM, geo.isMidPixOffset)
    .saveObj(KEY_BEAM_OFFSET, geo.midPixOffset.saveJson())
  );

  auto &cut = session->imageCut();
  top.saveObj(KEY_CUT, JsonObj()
    .saveUint(KEY_LEFT,   cut.left)
    .saveUint(KEY_TOP,    cut.top)
    .saveUint(KEY_RIGHT,  cut.right)
    .saveUint(KEY_BOTTOM, cut.bottom)
  );

  auto &trn = session->imageTransform();
  top.saveUint(KEY_TRANSFORM, trn.val);

  JsonArr arrFiles;
  // save file path relative to location of session
  for_i (numFiles()) {
    str absPath = getFile(i)->fileInfo().absoluteFilePath();
    str relPath = QDir::current().relativeFilePath(absPath);
    arrFiles.append(relPath);
  }

  top.saveArr(KEY_FILES, arrFiles);

  JsonArr arrSelectedFiles;
  for (uint i: collectedFromFiles())
    arrSelectedFiles.append((int)i);

  top.saveArr(KEY_SELECTED_FILES, arrSelectedFiles);

  if (hasCorrFile()) {
    str absPath = session->corrFile()->fileInfo().absoluteFilePath();
    str relPath = QDir::current().relativeFilePath(absPath);
    top.saveString(KEY_CORR_FILE, relPath);
  }

  top.saveUint(KEY_BG_DEGREE, bgPolynomialDegree());
  top.saveArr(KEY_BG_RANGES, bgRanges().saveJson());

  JsonArr arrReflections;
  for (auto &reflection: reflections())
    arrReflections.append(reflection->saveJson());

  top.saveArr(KEY_REFLECTIONS, arrReflections);

  return QJsonDocument(top).toJson();
}

void TheHub::loadSession(QFileInfo const& fileInfo) THROWS {
  QFile file(fileInfo.absoluteFilePath());
  RUNTIME_CHECK(file.open(QIODevice::ReadOnly), "Session file cannot be opened");
  QDir::setCurrent(fileInfo.absolutePath());
  loadSession(file.readAll());
}

void TheHub::loadSession(QByteArray const& json) THROWS {
  QJsonParseError parseError;
  QJsonDocument doc(QJsonDocument::fromJson(json,&parseError));
  RUNTIME_CHECK(QJsonParseError::NoError==parseError.error, "Error parsing session file");

  WaitCursor __;

  session->clear();

  core::JsonObj top(doc.object());

  auto files = top.loadArr(KEY_FILES);
  for (auto file: files) {
    str filePath = file.toString();
    QDir dir(filePath);
    RUNTIME_CHECK(dir.makeAbsolute(),str("Invalid file path: %1").arg(filePath));
    addFile(dir.absolutePath());
  }

  auto sels = top.loadArr(KEY_SELECTED_FILES,true);
  uint_vec selIndexes;
  for (auto sel: sels) {
    int i = sel.toInt(), index = qBound(0, i, files.count());
    RUNTIME_CHECK(i==index, str("Invalid selection index: %1").arg(i));
    selIndexes.append(index);
  }

  std::sort(selIndexes.begin(),selIndexes.end());
  int lastIndex = -1;
  for (uint index: selIndexes) {
    RUNTIME_CHECK(lastIndex < (int)index, str("Duplicate selection index"));
    lastIndex = index;
  }

  collectDatasetsFromFiles(selIndexes);
  tellFilesSelectedDatasetsChanged();

  setCorrFile(top.loadString(KEY_CORR_FILE,EMPTY_STR));

  auto det = top.loadObj(KEY_DETECTOR);
  setGeometry(
    det.loadReal(KEY_DISTANCE),
    det.loadReal(KEY_PIX_SIZE),
    det.loadBool(KEY_OFFSET_BEAM),
    det.loadIJ(KEY_BEAM_OFFSET)
  );

  auto cut = top.loadObj(KEY_CUT);
  uint x1 = cut.loadUint(KEY_LEFT),  y1 = cut.loadUint(KEY_TOP),
       x2 = cut.loadUint(KEY_RIGHT), y2 = cut.loadUint(KEY_BOTTOM);
  setImageCut(true,false,core::ImageCut(x1,y1,x2,y2));

  setImageRotate(core::ImageTransform(top.loadUint(KEY_TRANSFORM)));

  bgPolynomialDegree() = top.loadUint(KEY_BG_DEGREE);
  bgRanges().loadJson(top.loadArr(KEY_BG_RANGES));

  auto reflectionsObj = top.loadArr(KEY_REFLECTIONS);
  for_i (reflectionsObj.count()) {
    core::shp_Reflection reflection(new core::Reflection);
    reflection->loadJson(reflectionsObj.at(i).toObject());
    session->reflections().append(reflection);
  }

  emit reflectionsChanged();
}

void TheHub::addFile(rcstr filePath) THROWS {
  if (!filePath.isEmpty() && !session->hasFile(filePath)) {
    WaitCursor __;
    session->addFile(core::io::load(filePath));
    emit filesChanged();
  }
}

void TheHub::addFiles(str_lst filePaths) THROWS {
  WaitCursor __;

  for (auto &filePath: filePaths)
    addFile(filePath);
}

void TheHub::setCorrFile(rcstr filePath) THROWS {
  core::shp_File file;
  if (!filePath.isEmpty())
    file = core::io::load(filePath);

  bool isFile = !file.isNull();
  str fileName = isFile ? file->fileName() : EMPTY_STR;

  actions.remCorr->setEnabled(isFile);
  session->setCorrFile(file);
  emit corrFileName(fileName);
  enableCorrection(true);
}

void TheHub::enableCorrection(bool on) {
  session->enableCorr(on);
  emit corrEnabled(session->isCorrEnabled());
}

core::ImageCut const& TheHub::imageCut() const {
  return session->imageCut();
}

void TheHub::setImageCut(bool topLeft, bool linked, core::ImageCut const& margins) {
  session->setImageCut(topLeft,linked,margins);
  emit geometryChanged();
}

const core::Geometry &TheHub::geometry() const {
  return session->geometry();
}

void TheHub::setGeometry(qreal detectorDistance, qreal pixSize, bool isMidPixOffset, core::rcIJ midPixOffset) {
  session->setGeometry(detectorDistance,pixSize,isMidPixOffset,midPixOffset);
  emit geometryChanged();
}

void TheHub::setBackgroundPolynomialDegree(uint degree) {
  emit backgroundPolynomialDegree(degree);
}

void TheHub::setReflType(core::ePeakType type) {
  if (selectedReflection) {
    selectedReflection->setType(type);
    emit reflectionsChanged();
  }
}

void TheHub::addReflection(core::ePeakType type) {
  reflections().append(core::shp_Reflection(new core::Reflection(type)));
  emit reflectionsChanged();
}

void TheHub::remReflection(uint i) {
  auto &rs = reflections();
  rs.remove(i);
  if (rs.isEmpty())
    emit tellSelectedReflection(core::shp_Reflection());

  emit reflectionsChanged();
}

void TheHub::setFittingTab(int index) {
  emit fittingTab((fittingTab__=index));
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

  actions.rotateImage->setIcon(QIcon(rotateIconFile));
  actions.mirrorImage->setIcon(QIcon(mirrorIconFile));
  session->setImageTransformRotate(rot);
  setImageCut(true,false,session->imageCut()); // TODO make makeSafeCut(?)
  emit geometryChanged();
}

void TheHub::setImageMirror(bool on) {
  actions.mirrorImage->setChecked(on);
  session->setImageTransformMirror(on);
  emit geometryChanged();
}

void TheHub::setNorm(core::eNorm norm) {
  session->setNorm(norm);
  emit normChanged();
  emit geometryChanged();
}

//------------------------------------------------------------------------------
}
// eof
