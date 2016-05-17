// ************************************************************************** //
//
//  STeCa2:    StressTexCalculator ver. 2 REVIEW
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
#include "core_reflection_info.h"
#include "io/core_io.h"
#include "types/core_async.h"

#include <QSpinBox>
#include <QJsonDocument>
#include <QDir>

namespace gui {
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
  EXPECT(act->isCheckable())
  if (act) act->setChecked(readVariant(key,def).toBool());
}

void Settings::save(rcstr key, QAction* act) {
  EXPECT(act->isCheckable())
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

TheHub::TheHub()
: actions(*this), session(new core::Session())
, fixedIntenScaleImage_(false), fixedIntenScaleDgram_(false), avgCurveDgram_(false)
, filesModel(*this), datasetsModel(*this), reflectionsModel(*this)
, numGroupBy_(1) {
  configActions();
}

void TheHub::configActions() {
  actions.remFile->setEnabled(false);
  actions.remReflection->setEnabled(false);

  connect(actions.enableCorr, &QAction::toggled, [this](bool on) {
    tryEnableCorrection(on);
  });

  connect(actions.remCorr, &QAction::triggered, [this]() {
    setCorrFile(EMPTY_STR);
  });

  connect(actions.fixedIntenImageScale,&QAction::toggled,[this](bool on) {
    fixedIntenScaleImage_ = on;
    emit sigDisplayChanged();
  });

  connect(actions.fixedIntenDgramScale, &QAction::toggled, [this](bool on) {
    fixedIntenScaleDgram_ = on;
    emit sigDisplayChanged();
  });

  actions.avgCurveDgram->setChecked(false);
  connect(actions.avgCurveDgram, &QAction::toggled, [this](bool on) {
    avgCurveDgram_ = on;
    emit sigDisplayChanged();
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
  emit sigFilesChanged();

  if (0==numFiles())
    setImageCut(true, false, core::ImageCut());
}

bool TheHub::hasCorrFile() const {
  return session->hasCorrFile();
}

core::rcImage TheHub::corrImage() const {
  return session->corrImage();
}

core::shp_ImageLens TheHub::lensNoCut(core::rcImage image) const {
  return session->lens(image, collectedDatasets(), true, false);
}

core::shp_Lens TheHub::lens(core::rcDataset dataset) const {
  return session->lens(dataset,
                       dataset.datasets(),
                       true, true, session->norm());
}

core::shp_Lens TheHub::lensNoCut(core::rcDataset dataset) const {
  return session->lens(dataset,
                       dataset.datasets(),
                       true, false, session->norm());
}

core::AngleMap const& TheHub::angleMap(core::rcDataset dataset) const {
  return session->angleMap(dataset);
}

core::ReflectionInfos TheHub::makeReflectionInfos(core::rcReflection reflection, qreal betaStep,
                                                  core::rcRange gammaRange) {
  // TODO TODO TODO
  return session->makeReflectionInfos(
     collectedDatasets(), reflection, betaStep, gammaRange);
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

  top.saveUint(KEY_BG_DEGREE, bgPolyDegree());
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

  TakesLongTime __;

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

  core::Ranges bgRanges;
  bgRanges.loadJson(top.loadArr(KEY_BG_RANGES));
  setBgRanges(bgRanges);

  setBgPolyDegree(top.loadUint(KEY_BG_DEGREE));

  auto reflectionsObj = top.loadArr(KEY_REFLECTIONS);
  for_i (reflectionsObj.count()) {
    core::shp_Reflection reflection(new core::Reflection);
    reflection->loadJson(reflectionsObj.at(i).toObject());
    session->addReflection(reflection);
  }

  emit sigReflectionsChanged();
}

void TheHub::addFile(rcstr filePath) THROWS {
  if (!filePath.isEmpty() && !session->hasFile(filePath)) {
    {
      TakesLongTime __;
      session->addFile(core::io::load(filePath));
    }

    emit sigFilesChanged();
  }
}

void TheHub::addFiles(str_lst const& filePaths) THROWS {
  TakesLongTime __;

  for (auto &filePath: filePaths)
    addFile(filePath);
}

void TheHub::collectDatasetsFromFiles(uint_vec is, uint by) {
  session->collectDatasetsFromFiles(
    (collectFromFiles_ = is), (numGroupBy_ = by)
  );

  emit sigFilesSelected();
  emit sigDatasetsChanged();
}

void TheHub::collectDatasetsFromFiles(uint_vec is) {
  collectDatasetsFromFiles(is, numGroupBy_);
}

void TheHub::combineDatasetsBy(uint by) {
  collectDatasetsFromFiles(collectFromFiles_, by);
}

void TheHub::setCorrFile(rcstr filePath) THROWS {
  core::shp_File file;
  if (!filePath.isEmpty())
    file = core::io::load(filePath);

  session->setCorrFile(file);
  emit sigCorrFile(file);

  tryEnableCorrection(true);
}

void TheHub::tryEnableCorrection(bool on) {
  session->tryEnableCorr(on);
  emit sigCorrEnabled(session->isCorrEnabled());
}

core::ImageCut const& TheHub::imageCut() const {
  return session->imageCut();
}

void TheHub::setImageCut(bool topLeft, bool linked, core::ImageCut const& margins) {
  session->setImageCut(topLeft,linked,margins);
  emit sigGeometryChanged();
}

const core::Geometry &TheHub::geometry() const {
  return session->geometry();
}

void TheHub::setGeometry(qreal detectorDistance, qreal pixSize, bool isMidPixOffset, core::rcIJ midPixOffset) {
  session->setGeometry(detectorDistance,pixSize,isMidPixOffset,midPixOffset);
  emit sigGeometryChanged();
}

void TheHub::setBgRanges(core::rcRanges ranges) {
  session->setBgRanges(ranges);
  emit sigBgChanged();
}

void TheHub::addBgRange(core::rcRange range) {
  if (session->addBgRange(range))
    emit sigBgChanged();
}

void TheHub::remBgRange(core::rcRange range) {
  if (session->remBgRange(range))
    emit sigBgChanged();
}

void TheHub::setBgPolyDegree(uint degree) {
  session->setBgPolyDegree(degree);
  emit sigBgChanged();
}

void TheHub::setReflType(core::ePeakType type) {
  if (selectedReflection_) {
    selectedReflection_->setType(type);
    emit sigReflectionsChanged();
  }
}

void TheHub::addReflection(core::ePeakType type) {
  session->addReflection(core::shp_Reflection(new core::Reflection(type)));
  emit sigReflectionsChanged();
}

void TheHub::remReflection(uint i) {
  session->remReflection(i);
  if (session->reflections().isEmpty())
    tellSelectedReflection(core::shp_Reflection());

  emit sigReflectionsChanged();
}

void TheHub::setFittingTab(int index) {
  emit sigFittingTab((fittingTab__=index));
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
  emit sigGeometryChanged();
}

void TheHub::setImageMirror(bool on) {
  actions.mirrorImage->setChecked(on);
  session->setImageTransformMirror(on);
  emit sigGeometryChanged();
}

void TheHub::setNorm(core::eNorm norm) {
  session->setNorm(norm);
  emit sigNormChanged();
}

//------------------------------------------------------------------------------
}
// eof
