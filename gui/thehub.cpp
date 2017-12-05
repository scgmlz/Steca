// ************************************************************************** //
//
//  Steca2: stress and texture calculator
//
//! @file      gui/thehub.cpp
//! @brief     Implements ...
//!
//! @homepage  https://github.com/scgmlz/Steca2
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2017
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
// ************************************************************************** //

#include "thehub.h"
#include "calc/calc_reflection.h"
#include "calc/calc_reflection_info.h"
#include "config.h"
#include "io_io.h"
#include "mainwin.h"
#include "typ/typ_async.h"
#include "typ/typ_json.h"

#include <QDir>
#include <QJsonDocument>
#include <QMessageBox>
#include <QSpinBox>

namespace gui {

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
    if (act)
        act->setChecked(readVariant(key, def).toBool());
}

void Settings::save(rcstr key, QAction* act) {
    EXPECT(act->isCheckable())
    if (act)
        saveVariant(key, act->isChecked());
}

void Settings::read(rcstr key, QSpinBox* box, int def) {
    if (box)
        box->setValue(readVariant(key, def).toInt());
}

void Settings::save(rcstr key, QSpinBox* box) {
    if (box)
        saveVariant(key, box->value());
}

void Settings::read(rcstr key, QDoubleSpinBox* box, qreal def) {
    if (box)
        box->setValue(readVariant(key, def).toDouble());
}

void Settings::save(rcstr key, QDoubleSpinBox* box) {
    if (box)
        saveVariant(key, box->value());
}

bool Settings::readBool(rcstr key, bool def) {
    return readVariant(key, def).toBool();
}

void Settings::saveBool(rcstr key, bool val) {
    saveVariant(key, val);
}

qreal Settings::readReal(rcstr key, qreal def) {
    auto var = readVariant(key, QVariant());
    bool ok;
    qreal val = var.toDouble(&ok);
    return ok ? val : def;
}

void Settings::saveReal(rcstr key, qreal val) {
    saveVariant(key, val);
}

int Settings::readInt(rcstr key, int def) {
    auto var = readVariant(key, QVariant());
    bool ok;
    int val = var.toInt(&ok);
    return ok ? val : def;
}

void Settings::saveInt(rcstr key, int val) {
    saveVariant(key, val);
}

str Settings::readStr(rcstr key, rcstr def) {
    return readVariant(key, def).toString();
}

void Settings::saveStr(rcstr key, rcstr val) {
    saveVariant(key, val);
}

ReadFile::ReadFile(rcstr path) THROWS : super(path) {
    RUNTIME_CHECK(
        super::open(QIODevice::ReadOnly | QIODevice::Text),
        "Cannot open file for reading: " % path);
}

WriteFile::WriteFile(rcstr path) THROWS : super(path) {
    if (super::exists()) {
        if (QMessageBox::Yes
            != QMessageBox::question(nullptr, "File exists", "Overwrite " % path % " ?"))
            THROW_SILENT();
    }

    RUNTIME_CHECK(
        super::open(QIODevice::WriteOnly | QIODevice::Text),
        "Cannot open file for writing: " % path);
}

TheHub::TheHub()
    : actions(*this)
    , session_(new core::Session())
    , isFixedIntenImageScale_(false)
    , isFixedIntenDgramScale_(false)
    , isCombinedDgram_(false)
    , filesModel(*this)
    , datasetsModel(*this)
    , metadataModel(*this)
    , reflectionsModel(*this) {
    configActions();
}

void TheHub::configActions() {
    actions.remFile->setEnabled(false);
    actions.remReflection->setEnabled(false);

    connect(actions.enableCorr, &QAction::toggled, [this](bool on) { tryEnableCorrection(on); });

    connect(actions.remCorr, &QAction::triggered, [this]() { setCorrFile(EMPTY_STR); });

    connect(actions.fixedIntenImage, &QAction::toggled, [this](bool on) {
        isFixedIntenImageScale_ = on;
        emit sigDisplayChanged();
    });

    connect(actions.fixedIntenDgram, &QAction::toggled, [this](bool on) {
        isFixedIntenDgramScale_ = on;
        emit sigDisplayChanged();
    });

    actions.combinedDgram->setChecked(false);
    connect(actions.combinedDgram, &QAction::toggled, [this](bool on) {
        isCombinedDgram_ = on;
        emit sigDisplayChanged();
    });

    connect(actions.mirrorImage, &QAction::toggled, [this](bool on) { setImageMirror(on); });

    connect(actions.rotateImage, &QAction::triggered, [this]() {
        setImageRotate(session_->imageTransform().nextRotate());
    });
}

uint TheHub::numFiles() const {
    return session_->numFiles();
}

str TheHub::fileName(uint index) const {
    return getFile(index)->fileName();
}

str TheHub::filePath(uint index) const {
    return getFile(index)->fileInfo().absoluteFilePath();
}

data::shp_File TheHub::getFile(uint index) const {
    return session_->file(index);
}

void TheHub::remFile(uint i) {
    session_->remFile(i);
    emit sigFilesChanged();

    if (0 == numFiles())
        setImageCut(true, false, typ::ImageCut());
}

bool TheHub::hasCorrFile() const {
    return session_->hasCorrFile();
}

typ::shp_Image TheHub::corrImage() const {
    return session_->corrImage();
}

calc::shp_ImageLens TheHub::plainImageLens(typ::Image::rc image) const {
    return session_->imageLens(image, collectedDatasets(), true, false);
}

calc::shp_DatasetLens TheHub::datasetLens(data::Dataset::rc dataset) const {
    return session_->datasetLens(dataset, dataset.datasets(), session_->norm(), true, true);
}

typ::Curve TheHub::avgCurve(data::Datasets::rc datasets) const {
    return datasets.avgCurve(*session_);
}

calc::ReflectionInfos TheHub::makeReflectionInfos(
    calc::Reflection::rc reflection, uint gmaSlices, gma_rge::rc rgeGma, Progress* progress) {
    return session_->makeReflectionInfos(
        collectedDatasets(), reflection, gmaSlices, rgeGma, progress);
}

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

    auto& geo = session_->geometry();
    top.saveObj(
        config_key::DETECTOR,
        JsonObj()
            .savePreal(config_key::DET_DISTANCE, geo.detectorDistance)
            .savePreal(config_key::DET_PIX_SIZE, geo.pixSize)
            .saveObj(config_key::BEAM_OFFSET, geo.midPixOffset.saveJson()));

    auto& cut = session_->imageCut();
    top.saveObj(
        config_key::CUT,
        JsonObj()
            .saveUint(config_key::LEFT, cut.left)
            .saveUint(config_key::TOP, cut.top)
            .saveUint(config_key::RIGHT, cut.right)
            .saveUint(config_key::BOTTOM, cut.bottom));

    auto& trn = session_->imageTransform();
    top.saveUint(config_key::TRANSFORM, trn.val);

    JsonArr arrFiles;
    // save file path relative to location of session
    for_i (numFiles()) {
        str absPath = getFile(i)->fileInfo().absoluteFilePath();
        str relPath = QDir::current().relativeFilePath(absPath);
        arrFiles.append(relPath);
    }

    top.saveArr(config_key::FILES, arrFiles);

    JsonArr arrSelectedFiles;
    for (uint i : collectedFromFiles())
        arrSelectedFiles.append(to_i(i));

    top.saveArr(config_key::SELECTED_FILES, arrSelectedFiles);
    top.saveUint(config_key::COMBINE, datasetsGroupedBy_);

    if (hasCorrFile()) {
        str absPath = session_->corrFile()->fileInfo().absoluteFilePath();
        str relPath = QDir::current().relativeFilePath(absPath);
        top.saveString(config_key::CORR_FILE, relPath);
    }

    top.saveUint(config_key::BG_DEGREE, bgPolyDegree());
    top.saveArr(config_key::BG_RANGES, bgRanges().saveJson());
    top.saveBool(config_key::INTEN_SCALED_AVG, intenScaledAvg());
    top.savePreal(config_key::INTEN_SCALE, intenScale());

    JsonArr arrReflections;
    for (auto& reflection : reflections())
        arrReflections.append(reflection->saveJson());

    top.saveArr(config_key::REFLECTIONS, arrReflections);

    return QJsonDocument(top.sup()).toJson();
}

void TheHub::clearSession() {
    session_->clear();
    tellSessionCleared();
}

void TheHub::loadSession(QFileInfo const& fileInfo) THROWS {
    ReadFile file(fileInfo.absoluteFilePath());

    QDir::setCurrent(fileInfo.absolutePath());
    loadSession(file.readAll());
}

void TheHub::loadSession(QByteArray const& json) THROWS {
    QJsonParseError parseError;
    QJsonDocument doc(QJsonDocument::fromJson(json, &parseError));
    RUNTIME_CHECK(QJsonParseError::NoError == parseError.error, "Error parsing session file");

    TakesLongTime __;

    clearSession();

    typ::JsonObj top(doc.object());

    auto files = top.loadArr(config_key::FILES);
    for (auto file : files) {
        str filePath = file.toString();
        QDir dir(filePath);
        RUNTIME_CHECK(dir.makeAbsolute(), str("Invalid file path: %1").arg(filePath));
        addFile(dir.absolutePath());
    }

    auto sels = top.loadArr(config_key::SELECTED_FILES, true);
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

    collectDatasetsFromFiles(selIndexes, top.loadPint(config_key::COMBINE, 1));

    setCorrFile(top.loadString(config_key::CORR_FILE, EMPTY_STR));

    auto det = top.loadObj(config_key::DETECTOR);
    setGeometry(
        det.loadPreal(config_key::DET_DISTANCE), det.loadPreal(config_key::DET_PIX_SIZE),
        det.loadIJ(config_key::BEAM_OFFSET));

    auto cut = top.loadObj(config_key::CUT);
    uint x1 = cut.loadUint(config_key::LEFT), y1 = cut.loadUint(config_key::TOP),
         x2 = cut.loadUint(config_key::RIGHT), y2 = cut.loadUint(config_key::BOTTOM);
    setImageCut(true, false, typ::ImageCut(x1, y1, x2, y2));

    setImageRotate(typ::ImageTransform(top.loadUint(config_key::TRANSFORM)));

    typ::Ranges bgRanges;
    bgRanges.loadJson(top.loadArr(config_key::BG_RANGES));
    setBgRanges(bgRanges);

    setBgPolyDegree(top.loadUint(config_key::BG_DEGREE));

    setIntenScaleAvg(
        top.loadBool(config_key::INTEN_SCALED_AVG, true),
        top.loadPreal(config_key::INTEN_SCALE, preal(1)));

    auto reflectionsObj = top.loadArr(config_key::REFLECTIONS);
    for_i (reflectionsObj.count()) {
        calc::shp_Reflection reflection(new calc::Reflection);
        reflection->loadJson(reflectionsObj.objAt(i));
        session_->addReflection(reflection);
    }

    emit sigReflectionsChanged();
}

void TheHub::addFile(rcstr filePath) THROWS {
    if (!filePath.isEmpty() && !session_->hasFile(filePath)) {
        {
            TakesLongTime __;
            session_->addFile(io::load(filePath));
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
    session_->collectDatasetsFromFiles((collectFromFiles_ = is), (datasetsGroupedBy_ = by));
    emit sigFilesSelected();
    emit sigDatasetsChanged();
}

void TheHub::collectDatasetsFromFiles(uint_vec is) {
    collectDatasetsFromFiles(is, datasetsGroupedBy_);
}

void TheHub::combineDatasetsBy(pint by) {
    collectDatasetsFromFiles(collectFromFiles_, by);
}

gma_rge TheHub::collectedDatasetsRgeGma() const {
    return collectedDatasets().rgeGma(*session_);
}

void TheHub::setCorrFile(rcstr filePath) THROWS {
    data::shp_File file;
    if (!filePath.isEmpty())
        file = io::load(filePath);

    session_->setCorrFile(file);
    emit sigCorrFile(file);

    tryEnableCorrection(true);
}

void TheHub::tryEnableCorrection(bool on) {
    session_->tryEnableCorr(on);
    emit sigCorrEnabled(session_->isCorrEnabled());
}

typ::ImageCut::rc TheHub::imageCut() const {
    return session_->imageCut();
}

void TheHub::setImageCut(bool topLeft, bool linked, typ::ImageCut::rc cut) {
    session_->setImageCut(topLeft, linked, cut);
    emit sigGeometryChanged();
}

const typ::Geometry& TheHub::geometry() const {
    return session_->geometry();
}

void TheHub::setGeometry(preal detectorDistance, preal pixSize, typ::IJ::rc midPixOffset) {
    level_guard __(sigLevel_);
    if (sigLevel_ > 1)
        return;

    session_->setGeometry(detectorDistance, pixSize, midPixOffset);
    emit sigGeometryChanged();
}

typ::Range::rc TheHub::gammaRange() const {
    return session_->gammaRange();
}

void TheHub::setGammaRange(typ::Range::rc gammaRange) {
    session_->setGammaRange(gammaRange);
    emit sigGammaRange();
}

void TheHub::setBgRanges(typ::Ranges::rc ranges) {
    session_->setBgRanges(ranges);
    emit sigBgChanged();
}

void TheHub::addBgRange(typ::Range::rc range) {
    if (session_->addBgRange(range))
        emit sigBgChanged();
}

void TheHub::remBgRange(typ::Range::rc range) {
    if (session_->remBgRange(range))
        emit sigBgChanged();
}

void TheHub::setBgPolyDegree(uint degree) {
    session_->setBgPolyDegree(degree);
    emit sigBgChanged();
}

void TheHub::setIntenScaleAvg(bool avg, preal scale) {
    session_->setIntenScaleAvg(avg, scale);
    emit sigNormChanged(); // TODO instead of another signal
}

void TheHub::setReflType(fit::ePeakType type) {
    if (selectedReflection_) {
        selectedReflection_->setType(type);
        emit sigReflectionsChanged();
    }
}

void TheHub::addReflection(fit::ePeakType type) {
    session_->addReflection(calc::shp_Reflection(new calc::Reflection(type)));
    emit sigReflectionsChanged();
}

void TheHub::remReflection(uint i) {
    session_->remReflection(i);
    if (session_->reflections().isEmpty())
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
    session_->setImageTransformRotate(rot);
    setImageCut(true, false, session_->imageCut());
    emit sigGeometryChanged();
}

void TheHub::setImageMirror(bool on) {
    actions.mirrorImage->setChecked(on);
    session_->setImageTransformMirror(on);
    emit sigGeometryChanged();
}

void TheHub::setNorm(eNorm norm) {
    session_->setNorm(norm);
    emit sigNormChanged();
}
}
