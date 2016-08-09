// ************************************************************************** //
//
//  Steca2:    StressTextureCalculator ver. 2
//
//! @file      thehub.cpp
//!
//! @homepage  http://apps.jcns.fz-juelich.de/steca2
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016
//! @authors   Scientific Computing Group at MLZ Garching
//! @authors   Rebecca Brydon, Jan Burle,  Antti Soininen
//! @authors   Based on the original STeCa by Christian Randau
//
// ************************************************************************** //

#include "thehub.h"
#include "calc/calc_reflection.h"
#include "calc/calc_reflection_info.h"
#include "io/io_io.h"
#include "mainwin.h"
#include "typ/typ_async.h"
#include "typ/typ_json.h"

#include <QDir>
#include <QJsonDocument>
#include <QMessageBox>
#include <QSpinBox>

namespace gui {
//------------------------------------------------------------------------------

Settings::Settings(rcstr group) {
  setFallbacksEnabled(false);
  beginGroup(group);
}

Settings::~Settings() {
  endGroup();
}

QVariant Settings::readVariant(rcstr key, const QVariant& def) {
  auto val = value(key, def);
  return val;
}

void Settings::saveVariant(rcstr key, const QVariant& val) {
  setValue(key, val);
}

void Settings::read(rcstr key, QAction* act, bool def) {
  EXPECT(act->isCheckable())
  if (act) act->setChecked(readVariant(key, def).toBool());
}

void Settings::save(rcstr key, QAction* act) {
  EXPECT(act->isCheckable())
  if (act) saveVariant(key, act->isChecked());
}

void Settings::read(rcstr key, QSpinBox* box, int def) {
  if (box) box->setValue(readVariant(key, def).toInt());
}

void Settings::save(rcstr key, QSpinBox* box) {
  if (box) saveVariant(key, box->value());
}

void Settings::read(rcstr key, QDoubleSpinBox* box, qreal def) {
  if (box) box->setValue(readVariant(key, def).toDouble());
}

void Settings::save(rcstr key, QDoubleSpinBox* box) {
  if (box) saveVariant(key, box->value());
}

qreal Settings::readReal(rcstr key, qreal def) {
  auto var = readVariant(key, QVariant());
  if (QVariant::Double == var.type())
    return var.toDouble();
  return def;
}

void Settings::saveReal(rcstr key, qreal val) {
  saveVariant(key, val);
}

//------------------------------------------------------------------------------

ReadFile::ReadFile(rcstr path) THROWS : super(path) {
  RUNTIME_CHECK(super::open(QIODevice::ReadOnly),
    "Cannot open file for reading: " % path);
}

WriteFile::WriteFile(rcstr path) THROWS : super(path) {
  if (super::exists()) {
    if (QMessageBox::Yes != QMessageBox::question(nullptr,
        "File exists", "Overwrite " % path % " ?"))
      THROW_SILENT();
  }

  RUNTIME_CHECK(super::open(QIODevice::WriteOnly),
    "Cannot open file for writing: " % path);
}

//------------------------------------------------------------------------------

TheHub::TheHub()
: actions(*this), session(new core::Session())
, isFixedIntenImageScale_(false), isFixedIntenDgramScale_(false)
, isCombinedDgram_(false), filesModel(*this), datasetsModel(*this)
, reflectionsModel(*this)
{
  configActions();
}

void TheHub::configActions() {
  actions.remFile->setEnabled(false);
  actions.remReflection->setEnabled(false);

  connect(actions.enableCorr, &QAction::toggled,
          [this](bool on) { tryEnableCorrection(on); });

  connect(actions.remCorr, &QAction::triggered,
          [this]() { setCorrFile(EMPTY_STR); });

  connect(actions.fixedIntenImageScale, &QAction::toggled, [this](bool on) {
    isFixedIntenImageScale_ = on;
    emit sigDisplayChanged();
  });

  connect(actions.fixedIntenDgramScale, &QAction::toggled, [this](bool on) {
    isFixedIntenDgramScale_ = on;
    emit sigDisplayChanged();
  });

  actions.combinedDgram->setChecked(false);
  connect(actions.combinedDgram, &QAction::toggled, [this](bool on) {
    isCombinedDgram_ = on;
    emit sigDisplayChanged();
  });

  connect(actions.mirrorImage, &QAction::toggled,
          [this](bool on) { setImageMirror(on); });

  connect(actions.rotateImage, &QAction::triggered,
          [this]() { setImageRotate(session->imageTransform().nextRotate()); });
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

data::shp_File TheHub::getFile(uint index) const {
  return session->file(index);
}

void TheHub::remFile(uint i) {
  session->remFile(i);
  emit sigFilesChanged();

  if (0 == numFiles()) setImageCut(true, false, typ::ImageCut());
}

bool TheHub::hasCorrFile() const {
  return session->hasCorrFile();
}

typ::Image::rc TheHub::corrImage() const {
  return session->corrImage();
}

calc::shp_ImageLens TheHub::plainImageLens(typ::Image::rc image) const {
  return session->imageLens(image, collectedDatasets(), true, false);
}

calc::shp_DatasetLens TheHub::datasetLens(data::Dataset::rc dataset) const {
  return session->datasetLens(dataset, dataset.datasets(), session->norm(),
                              true, true);
}

calc::ReflectionInfos TheHub::makeReflectionInfos(
    calc::Reflection::rc reflection, gma_t gmaStep, gma_rge::rc rgeGma,
    Progress* progress)
{
  return session->makeReflectionInfos(collectedDatasets(), reflection,
                                      gmaStep, rgeGma, progress);
}

static str const KEY_FILES("files");
static str const KEY_SELECTED_FILES("selected files");
static str const KEY_COMBINE("combine");
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
  WriteFile file(fileInfo.filePath());

  QDir::setCurrent(fileInfo.absolutePath());
  auto written = file.write(saveSession());
  RUNTIME_CHECK(written >= 0, "Could not write session");
}

QByteArray TheHub::saveSession() const {
  using typ::JsonObj;
  using typ::JsonArr;

  JsonObj top;

  auto& geo = session->geometry();
  top.saveObj(KEY_DETECTOR, JsonObj()
      .savePreal(KEY_DISTANCE, geo.detectorDistance)
      .savePreal(KEY_PIX_SIZE, geo.pixSize)
      .saveBool(KEY_OFFSET_BEAM, geo.isMidPixOffset)
      .saveObj(KEY_BEAM_OFFSET, geo.midPixOffset.saveJson()));

  auto& cut = session->imageCut();
  top.saveObj(KEY_CUT, JsonObj()
      .saveUint(KEY_LEFT, cut.left)
      .saveUint(KEY_TOP, cut.top)
      .saveUint(KEY_RIGHT, cut.right)
      .saveUint(KEY_BOTTOM, cut.bottom));

  auto& trn = session->imageTransform();
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
  for (uint i : collectedFromFiles())
    arrSelectedFiles.append(to_i(i));

  top.saveArr(KEY_SELECTED_FILES, arrSelectedFiles);
  top.saveUint(KEY_COMBINE, datasetsGroupedBy_);

  if (hasCorrFile()) {
    str absPath = session->corrFile()->fileInfo().absoluteFilePath();
    str relPath = QDir::current().relativeFilePath(absPath);
    top.saveString(KEY_CORR_FILE, relPath);
  }

  top.saveUint(KEY_BG_DEGREE, bgPolyDegree());
  top.saveArr(KEY_BG_RANGES, bgRanges().saveJson());

  JsonArr arrReflections;
  for (auto& reflection : reflections())
    arrReflections.append(reflection->saveJson());

  top.saveArr(KEY_REFLECTIONS, arrReflections);

  return QJsonDocument(top.sup()).toJson();
}

void TheHub::loadSession(QFileInfo const& fileInfo) THROWS {
  ReadFile file(fileInfo.absoluteFilePath());

  QDir::setCurrent(fileInfo.absolutePath());
  loadSession(file.readAll());
}

void TheHub::loadSession(QByteArray const& json) THROWS {
  QJsonParseError parseError;
  QJsonDocument   doc(QJsonDocument::fromJson(json, &parseError));
  RUNTIME_CHECK(QJsonParseError::NoError == parseError.error,
                "Error parsing session file");

  TakesLongTime __;

  session->clear();

  typ::JsonObj top(doc.object());

  auto files = top.loadArr(KEY_FILES);
  for (auto file : files) {
    str  filePath = file.toString();
    QDir dir(filePath);
    RUNTIME_CHECK(dir.makeAbsolute(),
                  str("Invalid file path: %1").arg(filePath));
    addFile(dir.absolutePath());
  }

  auto     sels = top.loadArr(KEY_SELECTED_FILES, true);
  uint_vec selIndexes;
  for (auto sel : sels) {
    int i = sel.toInt(), index = qBound(0, i, to_i(files.count()));
    RUNTIME_CHECK(i == index, str("Invalid selection index: %1").arg(i));
    selIndexes.append(to_u(index));
  }

  std::sort(selIndexes.begin(), selIndexes.end());
  int lastIndex = -1;
  for (uint index : selIndexes) {
    RUNTIME_CHECK(lastIndex < to_i(index), str("Duplicate selection index"));
    lastIndex = to_i(index);
  }

  collectDatasetsFromFiles(selIndexes,top.loadPint(KEY_COMBINE,1));

  setCorrFile(top.loadString(KEY_CORR_FILE, EMPTY_STR));

  auto det = top.loadObj(KEY_DETECTOR);
  setGeometry(det.loadPreal(KEY_DISTANCE), det.loadPreal(KEY_PIX_SIZE),
              det.loadBool(KEY_OFFSET_BEAM), det.loadIJ(KEY_BEAM_OFFSET));

  auto cut = top.loadObj(KEY_CUT);
  uint x1 = cut.loadUint(KEY_LEFT), y1 = cut.loadUint(KEY_TOP),
       x2 = cut.loadUint(KEY_RIGHT), y2 = cut.loadUint(KEY_BOTTOM);
  setImageCut(true, false, typ::ImageCut(x1, y1, x2, y2));

  setImageRotate(typ::ImageTransform(top.loadUint(KEY_TRANSFORM)));

  typ::Ranges bgRanges;
  bgRanges.loadJson(top.loadArr(KEY_BG_RANGES));
  setBgRanges(bgRanges);

  setBgPolyDegree(top.loadUint(KEY_BG_DEGREE));

  auto reflectionsObj = top.loadArr(KEY_REFLECTIONS);
  for_i (reflectionsObj.count()) {
    calc::shp_Reflection reflection(new calc::Reflection);
    reflection->loadJson(reflectionsObj.objAt(i));
    session->addReflection(reflection);
  }

  emit sigReflectionsChanged();
}

void TheHub::addFile(rcstr filePath) THROWS {
  if (!filePath.isEmpty() && !session->hasFile(filePath)) {
    {
      TakesLongTime __;
      session->addFile(io::load(filePath));
    }

    emit sigFilesChanged();
  }
}

void TheHub::addFiles(str_lst::rc filePaths) THROWS {
  TakesLongTime __;

  for (auto& filePath : filePaths)
    addFile(filePath);
}

void TheHub::collectDatasetsFromFiles(uint_vec is, pint by) {
  session->collectDatasetsFromFiles((collectFromFiles_  = is),
                                    (datasetsGroupedBy_ = by));
  emit sigFilesSelected();
  emit sigDatasetsChanged();
}

void TheHub::collectDatasetsFromFiles(uint_vec is) {
  collectDatasetsFromFiles(is, datasetsGroupedBy_);
}

void TheHub::combineDatasetsBy(pint by) {
  collectDatasetsFromFiles(collectFromFiles_, by);
}

void TheHub::setCorrFile(rcstr filePath) THROWS {
  data::shp_File file;
  if (!filePath.isEmpty()) file = io::load(filePath);

  session->setCorrFile(file);
  emit sigCorrFile(file);

  tryEnableCorrection(true);
}

void TheHub::tryEnableCorrection(bool on) {
  session->tryEnableCorr(on);
  emit sigCorrEnabled(session->isCorrEnabled());
}

typ::ImageCut::rc TheHub::imageCut() const {
  return session->imageCut();
}

void TheHub::setImageCut(bool topLeft, bool linked,
                         typ::ImageCut::rc margins) {
  session->setImageCut(topLeft, linked, margins);
  emit sigGeometryChanged();
}

const typ::Geometry& TheHub::geometry() const {
  return session->geometry();
}

void TheHub::setGeometry(preal detectorDistance, preal pixSize,
                         bool isMidPixOffset, typ::IJ::rc midPixOffset) {
  level_guard __(sigLevel_);
  if (sigLevel_ > 1)
    return;

  session->setGeometry(detectorDistance, pixSize, isMidPixOffset, midPixOffset);
  emit sigGeometryChanged();
}

void TheHub::setBgRanges(typ::Ranges::rc ranges) {
  session->setBgRanges(ranges);
  emit sigBgChanged();
}

void TheHub::addBgRange(typ::Range::rc range) {
  if (session->addBgRange(range)) emit sigBgChanged();
}

void TheHub::remBgRange(typ::Range::rc range) {
  if (session->remBgRange(range)) emit sigBgChanged();
}

void TheHub::setBgPolyDegree(uint degree) {
  session->setBgPolyDegree(degree);
  emit sigBgChanged();
}

void TheHub::setReflType(fit::ePeakType type) {
  if (selectedReflection_) {
    selectedReflection_->setType(type);
    emit sigReflectionsChanged();
  }
}

void TheHub::addReflection(fit::ePeakType type) {
  session->addReflection(calc::shp_Reflection(new calc::Reflection(type)));
  emit sigReflectionsChanged();
}

void TheHub::remReflection(uint i) {
  session->remReflection(i);
  if (session->reflections().isEmpty())
    tellSelectedReflection(calc::shp_Reflection());

  emit sigReflectionsChanged();
}

void TheHub::setFittingTab(eFittingTab tab) {
  emit sigFittingTab((fittingTab_ = tab));
}

void TheHub::setImageRotate(typ::ImageTransform rot) {
  pcstr rotateIconFile, mirrorIconFile;

  switch (rot.val & 3) {
  default /* case */:
    rotateIconFile = ":/icon/rotate0";
    mirrorIconFile = ":/icon/mirrorHorz";
    break;
  case 1:
    rotateIconFile = ":/icon/rotate1";
    mirrorIconFile = ":/icon/mirrorVert";
    break;
  case 2:
    rotateIconFile = ":/icon/rotate2";
    mirrorIconFile = ":/icon/mirrorHorz";
    break;
  case 3:
    rotateIconFile = ":/icon/rotate3";
    mirrorIconFile = ":/icon/mirrorVert";
    break;
  }

  actions.rotateImage->setIcon(QIcon(rotateIconFile));
  actions.mirrorImage->setIcon(QIcon(mirrorIconFile));
  session->setImageTransformRotate(rot);
  setImageCut(true, false, session->imageCut());
  emit sigGeometryChanged();
}

void TheHub::setImageMirror(bool on) {
  actions.mirrorImage->setChecked(on);
  session->setImageTransformMirror(on);
  emit sigGeometryChanged();
}

void TheHub::setNorm(eNorm norm) {
  session->setNorm(norm);
  emit sigNormChanged();
}

//------------------------------------------------------------------------------
}
// eof
